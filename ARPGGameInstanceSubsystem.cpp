// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameInstanceSubsystem.h"


#include "APRGGameSaver.h"

#include "Kismet/GameplayStatics.h"
#include "GameItemWidget.h"
#include "APRGGameSaver.h"
#include "ARPGPlayerController.h"
#include "ARPGGameItemsManagerComponent.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/KismetTextLibrary.h"

void UARPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UARPGGameInstanceSubsystem::Deinitialize()
{
    Super::Deinitialize();
}


TArray<FArchiveInfoStruct> UARPGGameInstanceSubsystem::GetArchiveInfos()
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

    return ArchiveManager->ArchiveInfos;
}

AARPGMainCharacter* UARPGGameInstanceSubsystem::GetMainCharacter(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (UARPGGameInstanceSubsystem::Get(World))
    {
        return World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>()->MainCharacter.Get();
    }
    return nullptr;
}


AARPGPlayerController* UARPGGameInstanceSubsystem::GetMainCharacterController(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (UARPGGameInstanceSubsystem::Get(World))
    {
        return UARPGGameInstanceSubsystem::Get(World)->MainCharacterController.Get();
    }
    return nullptr;
}

UARPGStatusWidget* UARPGGameInstanceSubsystem::GetMainCharacterStatusWidget(const UObject* WorldContextObject)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (UARPGGameInstanceSubsystem::Get(World))
    {
        return UARPGGameInstanceSubsystem::Get(World)->StatusWidget.Get();
    }
    return nullptr;
}

void UARPGGameInstanceSubsystem::ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title,
                                            FText Content)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
    if (UARPGGameInstanceSubsystem::Get(World))
    {
        const auto StatusWidget = UARPGGameInstanceSubsystem::Get(World)->GetMainCharacterStatusWidget(World);
        if (StatusWidget)
        {
            StatusWidget->ShowNotify(Icon, Title, Content);
        }
        else
        {
            PrintLogToScreen(UKismetTextLibrary::Conv_TextToString(Title));
        }
    }
}


bool UARPGGameInstanceSubsystem::SaveArchive(FString ArchiveName)
{
    OnGameSaving.Broadcast();
    GameSaver = Cast<UAPRGGameSaver>(UGameplayStatics::CreateSaveGameObject(UAPRGGameSaver::StaticClass()));
    GameSaver->CurrentMap = CurrentStreamingLevelName;

    if (MainCharacter.IsValid())
    {
        GameSaver->MainCharacterTransform = MainCharacter->GetTransform();
    }else
    {
        GameSaver->MainCharacterTransform = UGameplayStatics::GetPlayerCharacter(GetWorld(),0)->GetTransform();
    }
    
    GameSaver->Time = FDateTime::Now();

    for (int i = 0; i < ArchiveManager->ArchiveInfos.Num(); ++i)
    {
        FArchiveInfoStruct& ArchiveInfoStruct = ArchiveManager->ArchiveInfos[i];
        if (ArchiveInfoStruct.SlotName == ArchiveName)
        {

            //保存道具
            TArray<AActor*> GameItems;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameItem::StaticClass(), GameItems);
            for (AActor* Actor : GameItems)
            {
                AGameItem* GameItem = Cast<AGameItem>(Actor);
                if (GameItem)
                {
                    FGameItemArchiveStruct GameItemArchiveStruct;
                    GameItemArchiveStruct.GameItemClass = GetPathNameSafe(GameItem->GetClass());
                    GameItemArchiveStruct.Number = GameItem->Number;
                    GameItemArchiveStruct.IsInBag = GameItem->IsInBag;
                    GameItemArchiveStruct.Transform = GameItem->GetActorTransform();
                    GameSaver->GameItems.Add(GameItemArchiveStruct);
                }
            }

            //保存角色
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


    //销毁所有角色
    TArray<AActor*> OutActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPGCharacter::StaticClass(), OutActors);
    for (auto Actor : OutActors)
    {
        Actor->Destroy();
    }

    //销毁所有道具
    TArray<AActor*> GameItems;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameItem::StaticClass(), GameItems);
    for (AActor* Actor : GameItems)
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
            if (MainCharacter.Get())
            {
                UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Character);
            }
            else
            {
                PrintLogToScreen(TEXT("错误，主角不是AARPGMainCharacter子类"));
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


    
    //还原道具
    TArray<AGameItem*> Bag;
    for (auto GameItemArchiveStruct : GameSaver->GameItems)
    {
        AGameItem* GameItem = Cast<AGameItem>(
            GetWorld()->SpawnActor(LoadClass<AGameItem>(nullptr, *GameItemArchiveStruct.GameItemClass.ToString()),
                                   &GameItemArchiveStruct.Transform));
        if (GameItem)
        {
            if (GameItemArchiveStruct.IsInBag)
            {
                GameItem->Number = GameItemArchiveStruct.Number;
                Bag.Emplace(GameItem->BeTaken());
            }
        }
    }


    if (MainCharacter.Get())
    {
        MainCharacter->GetGameItemsManagerComponent()->SetBag(Bag);
    }
    UE_LOG(LogTemp, Warning, TEXT("刷新物品和背包"));

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

void UARPGGameInstanceSubsystem::PrintLogToScreen(FString Message, float Time, FColor Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, Time, Color, Message);
    }
}

UARPGGameInstanceSubsystem* UARPGGameInstanceSubsystem::Get(UWorld* World)
{
    if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
    {
        return World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>();
    }

    return nullptr;
}
