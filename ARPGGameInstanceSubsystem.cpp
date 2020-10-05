// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameInstanceSubsystem.h"


#include "APRGGameSaver.h"
#include "PromptWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameItemWidget.h"
#include "APRGGameSaver.h"
#include "ARPGPlayerController.h"
#include "ARPGGameItemsManagerComponent.h"
#include "Engine/LevelStreamingDynamic.h"

void UARPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UARPGGameInstanceSubsystem::Deinitialize()
{
    Super::Deinitialize();
}


UAPRGArchiveManager* UARPGGameInstanceSubsystem::GetArchiveManager()
{
    if (!ArchiveManager)
    {
        ArchiveManager = Cast<UAPRGArchiveManager>(UGameplayStatics::LoadGameFromSlot(ArchiveManageSlot, 0));
        if (!ArchiveManager)
        {
            ArchiveManager = Cast<UAPRGArchiveManager>(
                UGameplayStatics::CreateSaveGameObject(UAPRGArchiveManager::StaticClass()));
            if (ArchiveManager)
            {
                for (int i = 0; i < 128; ++i)
                {
                    FArchiveInfoStruct ArchiveInfoStruct;
                    ArchiveInfoStruct.SlotName = FString(TEXT("Save")).Append(FString::FromInt(i));
                    ArchiveInfoStruct.IsArchiveValid = false;
                    ArchiveManager->ArchiveInfos.Emplace(ArchiveInfoStruct);
                }
                UGameplayStatics::SaveGameToSlot(ArchiveManager, ArchiveManageSlot, 0);
            }
        }
    }

    return ArchiveManager;
}

void UARPGGameInstanceSubsystem::ShowNotify(UTexture2D* Icon, FText Title, FText Content) const
{
    if (StatusWidget)
    {
        StatusWidget->ShowNotify(Icon, Title, Content);
    }
}


bool UARPGGameInstanceSubsystem::SaveArchive(FString ArchiveName)
{
    OnGameSaving.Broadcast();
    GameSaver = Cast<UAPRGGameSaver>(UGameplayStatics::CreateSaveGameObject(UAPRGGameSaver::StaticClass()));
    GameSaver->CurrentMap = CurrentStreamingLevelName;
    GameSaver->MainCharacterTransform = MainCharacter->GetTransform();
    GameSaver->Time = FDateTime::Now();

    for (int i = 0; i < ArchiveManager->ArchiveInfos.Num(); ++i)
    {
        FArchiveInfoStruct& ArchiveInfoStruct = ArchiveManager->ArchiveInfos[i];
        if (ArchiveInfoStruct.SlotName == ArchiveName)
        {
            //保存背包数据
            const auto GameItemsManagerComponent = MainCharacter->GetGameItemsManagerComponent();
            for (auto GameItem : GameItemsManagerComponent->GetAllGameItemsInBag())
            {
                TSharedPtr<FGameItemArchiveStruct> GameItemArchiveStruct(new FGameItemArchiveStruct());
                GameItemArchiveStruct->GameItemClass = GetPathNameSafe(GameItem->GetClass());
                
                GameItemArchiveStruct->Number = GameItem->Number;
                GameSaver->GameItems.Emplace(*GameItemArchiveStruct);
            }

            //保存所有角色
            TArray<AActor*> OutActors;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPGCharacter::StaticClass(), OutActors);
            for (auto Actor : OutActors)
            {
                AARPGCharacter* Character = Cast<AARPGCharacter>(Actor);
                if (!Character || !Character->GetCharacterStatusComponent())
                {
                    return false;
                }
                UCharacterStatusComponent* CharacterStatusComponent = Character->GetCharacterStatusComponent();
                TSharedPtr<FCharacterArchiveStruct> CharacterArchiveStruct(new FCharacterArchiveStruct());

                CharacterArchiveStruct->CharacterClassPath = GetPathNameSafe(Character->GetClass());
                CharacterArchiveStruct->CharacterName = CharacterStatusComponent->GetCharacterName();
                CharacterArchiveStruct->Level = CharacterStatusComponent->GetLevel();
                CharacterArchiveStruct->CurrentHP = CharacterStatusComponent->GetCurrentHP();
                CharacterArchiveStruct->CurrentSP = CharacterStatusComponent->GetCurrentSP();
                CharacterArchiveStruct->Coins = CharacterStatusComponent->GetCoins();
                CharacterArchiveStruct->AttackSpecialty = CharacterStatusComponent->GetStaminaSpecialty();
                CharacterArchiveStruct->AttackSpecialty = CharacterStatusComponent->GetAttackSpecialty();
                CharacterArchiveStruct->DefenseSpecialty = CharacterStatusComponent->GetDefenseSpecialty();
                CharacterArchiveStruct->ToughnessSpecialty = CharacterStatusComponent->GetToughnessSpecialty();
                CharacterArchiveStruct->CharacterState = CharacterStatusComponent->GetCharacterState();
                CharacterArchiveStruct->CharacterTransform = Character->GetActorTransform();

                GameSaver->Characters.Emplace(*CharacterArchiveStruct);
            }
            FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate;
            AsyncSaveGameToSlotDelegate.BindLambda([&](const FString&, const int32, bool)
            {
                ArchiveInfoStruct.IsArchiveValid = true;
                ArchiveInfoStruct.Timestamp = FDateTime::Now().ToString();

                UGameplayStatics::SaveGameToSlot(ArchiveManager, ArchiveManageSlot, 0);
                OnGameSaveSuccess.Broadcast();
            });
            UGameplayStatics::AsyncSaveGameToSlot(GameSaver, ArchiveName, 0, AsyncSaveGameToSlotDelegate);
            return true;
        }
    }

    return false;
}

void UARPGGameInstanceSubsystem::LoadArchive(FString ArchiveName)
{
    OnGameLoading.Broadcast();

    FAsyncLoadGameFromSlotDelegate LoadGameFromSlotDelegate;
    LoadGameFromSlotDelegate.BindUObject(this, &UARPGGameInstanceSubsystem::OnArchiveLoaded);
    UGameplayStatics::AsyncLoadGameFromSlot(ArchiveName, 0, LoadGameFromSlotDelegate);
}


void UARPGGameInstanceSubsystem::OnArchiveLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame)
{
    if (!SaveGame)
    {
        return;
    }
    GameSaver = Cast<UAPRGGameSaver>(SaveGame);

    if (GameSaver->CurrentMap != CurrentStreamingLevelName)
    {
        FLatentActionInfo LatentInfo;

        OnLevelLoading.Broadcast();
        UGameplayStatics::LoadStreamLevel(GetWorld(), GameSaver->CurrentMap, true, false, LatentInfo);
        UGameplayStatics::GetStreamingLevel(GetWorld(), GameSaver->CurrentMap)->OnLevelLoaded.AddDynamic(
            this, &UARPGGameInstanceSubsystem::OnLevelLoaded);
    }
    else
    {
        OnLevelLoaded();
    }
}

void UARPGGameInstanceSubsystem::OnLevelLoaded()
{
    OnLevelLoadSuccess.Broadcast();
    MainCharacter->SetActorTransform(GameSaver->MainCharacterTransform);


    //还原背包数据
    TArray<AGameItem*> Bag;
    for (auto ItemArchiveStruct : GameSaver->GameItems)
    {
        AGameItem* GameItem = Cast<AGameItem>(
            GetWorld()->SpawnActor(LoadClass<AGameItem>(nullptr, *ItemArchiveStruct.GameItemClass.ToString())));
        Bag.Emplace(GameItem->BeTaken());
        UE_LOG(LogTemp, Warning, TEXT("%s物品已重新生成"), *GameItem->ItemName.ToString());
    }
    MainCharacter->GetGameItemsManagerComponent()->SetBag(Bag);
    UE_LOG(LogTemp, Warning, TEXT("刷新背包"));

    //销毁所有角色
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPGCharacter::StaticClass(), OutActors);
    for (auto Actor : OutActors)
    {
        Actor->Destroy();
    }


    //还原角色
    for (auto CharacterArchiveStruct : GameSaver->Characters)
    {
        auto Character = Cast<AARPGCharacter>(GetWorld()->SpawnActor(
            LoadClass<AARPGCharacter>(nullptr, *CharacterArchiveStruct.CharacterClassPath.ToString()),
            &CharacterArchiveStruct.CharacterTransform));
        Character->SetCharacterName(CharacterArchiveStruct.CharacterName);
        if (Character->GetCharacterName() == "MainCharacter")
        {
            MainCharacter = Cast<AARPGMainCharacter>(Character);
            if (MainCharacter)
            {
                UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Character);
            }
            else
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red,TEXT("错误，主角不是AARPGMainCharacter子类"));
                }
            }
        }
        else
        {
            Character->SpawnDefaultController();
        }

        Character->GetCharacterStatusComponent()->SetLevel(CharacterArchiveStruct.Level);
        Character->GetCharacterStatusComponent()->SetCoins(CharacterArchiveStruct.Coins);
        Character->GetCharacterStatusComponent()->SetHealthSpecialty(CharacterArchiveStruct.HealthSpecialty);
        Character->GetCharacterStatusComponent()->SetStaminaSpecialty(CharacterArchiveStruct.StaminaSpecialty);
        Character->GetCharacterStatusComponent()->SetAttackSpecialty(CharacterArchiveStruct.AttackSpecialty);
        Character->GetCharacterStatusComponent()->SetDefenseSpecialty(CharacterArchiveStruct.DefenseSpecialty);
        Character->GetCharacterStatusComponent()->SetToughnessSpecialty(CharacterArchiveStruct.ToughnessSpecialty);
        Character->GetCharacterStatusComponent()->SetCurrentHP(CharacterArchiveStruct.CurrentHP);
        Character->GetCharacterStatusComponent()->SetCurrentSP(CharacterArchiveStruct.CurrentSP);
        Character->GetCharacterStatusComponent()->SetCharacterState(CharacterArchiveStruct.CharacterState);
        UE_LOG(LogTemp, Warning, TEXT("%s角色已重新生成"), *Character->GetCharacterName().ToString());
    }
    UE_LOG(LogTemp, Warning, TEXT("存档加载完成"));
    OnGameLoadSuccess.Broadcast();
}

void UARPGGameInstanceSubsystem::RandomChoice(float chance, EChoice& Choice)
{
    if (chance >= FMath::FRand())
    {
        Choice = EChoice::ChoiceA;
    }
    else
    {
        Choice = EChoice::ChoiceB;
    }
}

void UARPGGameInstanceSubsystem::PrintLogToScreen(FString Message,float Time, FColor Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Time, Color,Message);
    }
}

