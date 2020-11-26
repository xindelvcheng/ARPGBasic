// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameInstanceSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "BlueprintGraph/Public/BlueprintActionDatabaseRegistrar.h"
#include "KismetCompiler/Public/KismetCompiler.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/KismetTextLibrary.h"


#include "APRGGameSaver.h"
#include "GameItemWidget.h"
#include "ARPGPlayerController.h"
#include "ARPGGameItemsManagerComponent.h"

#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"



void UARPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 初始化存档列表
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

    FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UARPGGameInstanceSubsystem::Tick));
}

void UARPGGameInstanceSubsystem::Deinitialize()
{
    Super::Deinitialize();
}


TArray<FArchiveInfoStruct> UARPGGameInstanceSubsystem::GetArchiveInfos() const
{
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
    }
    else
    {
        GameSaver->MainCharacterTransform = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetTransform();
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
                    FGameItemArchiveStruct GameItemArchiveStruct = {
                        GameItem->GetClass(),
                        GameItem->Number,
                        GameItem->IsInBag,
                        GameItem->GetActorTransform()
                    };
                    GameSaver->GameItems.Emplace(std::move(GameItemArchiveStruct));
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
                UTranscendentalCombatComponent* CharacterCombatComponent = Character->GetCharacterCombatComponent();
                TArray<TSoftClassPtr<ATranscendentalLawsSystem>> TranscendentalLawsSystemClasses;
                for (ATranscendentalLawsSystem* TranscendentalLawsSystem : CharacterCombatComponent->
                     GetAllTranscendentalLawsSystems())
                {
                    TranscendentalLawsSystemClasses.Emplace(TranscendentalLawsSystem->GetClass()->GetPathName());
                }

                FCharacterArchiveStruct CharacterArchiveStruct = {
                    Character->GetClass(),
                    Character->GetTransform(),
                    Character->CharacterName,
                    Character->GetCharacterDisplayName(),
                    CharacterStatusComponent->GetLevel(),
                    CharacterStatusComponent->GetCoins(),
                    CharacterStatusComponent->GetHealthSpecialty(),
                    CharacterStatusComponent->GetStaminaSpecialty(),
                    CharacterStatusComponent->GetAttackSpecialty(),
                    CharacterStatusComponent->GetDefenseSpecialty(),
                    CharacterStatusComponent->GetToughnessSpecialty(),
                    CharacterStatusComponent->GetCurrentHP(),
                    CharacterStatusComponent->GetCurrentSP(),
                    CharacterStatusComponent->GetCharacterState(),
                    std::move(TranscendentalLawsSystemClasses)
                };

                GameSaver->Characters.Emplace(std::move(CharacterArchiveStruct));
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

    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    //还原角色
    for (auto CharacterArchiveStruct : GameSaver->Characters)
    {
        auto CharacterClass = CharacterArchiveStruct.CharacterClassPath.LoadSynchronous();
        checkf(GetWorld(), TEXT("World is nullptr!"));
        auto Actor = GetWorld()->SpawnActor(CharacterClass, &CharacterArchiveStruct.CharacterTransform,
                                            ActorSpawnParameters);
        auto Character = Cast<AARPGCharacter>(Actor);
        if (Character->CharacterName == "MainCharacter")
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

        TArray<ATranscendentalLawsSystem*> TranscendentalLawsSystems;
        for (auto TranscendentalLawsSystemSoftClass : CharacterArchiveStruct.TranscendentalLawsSystemClasses)
        {
            ATranscendentalLawsSystem* TranscendentalLawsSystem = Cast<ATranscendentalLawsSystem>(
                GetWorld()->SpawnActor(TranscendentalLawsSystemSoftClass.LoadSynchronous()));
            TranscendentalLawsSystem->Init(Character, Character->GetCharacterCombatComponent());
            TranscendentalLawsSystems.Emplace(TranscendentalLawsSystem);
        }
        Character->GetCharacterCombatComponent()->SetTranscendentalLawsSystems(std::move(TranscendentalLawsSystems));

        UE_LOG(LogTemp, Warning, TEXT("%s角色已重新生成"), *Character->CharacterName.ToString());
    }


    //还原道具
    TArray<AGameItem*> Bag;

    for (auto GameItemArchiveStruct : GameSaver->GameItems)
    {
        AGameItem* GameItem = Cast<AGameItem>(
            GetWorld()->SpawnActor(LoadClass<AGameItem>(nullptr, *GameItemArchiveStruct.GameItemClass.ToString()),
                                   &GameItemArchiveStruct.Transform, ActorSpawnParameters));
        if (GameItem)
        {
            if (GameItemArchiveStruct.IsInBag)
            {
                GameItem->Number = GameItemArchiveStruct.Number;
                Bag.Emplace(GameItem->BeTaken(MainCharacter.Get()));
            }
        }
    }

    MainCharacter->GetGameItemsManagerComponent()->SetBag(Bag);
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

FTransform UARPGGameInstanceSubsystem::GetActorNearPositionTransform(AActor* OriginActor,
                                                                     const FVector LocationOffset,
                                                                     const FRotator RotationOffset)
{
    return FTransform{
        OriginActor->GetActorRotation() + RotationOffset,
        OriginActor->GetActorLocation() + OriginActor->GetActorForwardVector() * LocationOffset.X + OriginActor->
        GetActorRightVector() * LocationOffset.Y + OriginActor->GetActorUpVector() * LocationOffset.Z
    };
}

UARPGGameInstanceSubsystem* UARPGGameInstanceSubsystem::Get(UWorld* World)
{
    if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
    {
        return World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>();
    }

    return nullptr;
}

void UARPGGameInstanceSubsystem::MoveActorTowardsDirection(AActor* Actor, FVector Direction,
                                                           FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                           float Duration)
{
    if (!Actor)
    {
        return;
    }

    if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
    {
        auto Record = UMoveActorTowardsDirectionRecord::CreateRecord(Actor, Direction, MoveFinishDelegate, MoveRate, Duration);
        Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem,&UARPGGameInstanceSubsystem::BindToMoveFinish);
        GameInstanceSubsystem->MoveRecords.Emplace(Record);
    }
}

void UARPGGameInstanceSubsystem::MoveActorTowardsDirectionFinishOnCollision(AActor* Actor, FVector Direction,TArray<AActor*> IgnoreActors,
                                                                            FCollisionDelegate OnCollision, float MoveRate, float Duration,bool ShouldStopAfterFirstCollision)
{
    if (!Actor)
    {
        return;
    }
    if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
    {
        auto Record = UMoveActorTowardDirectionFinishOnCollision::CreateRecord(
            Actor, Direction,IgnoreActors, OnCollision,MoveRate, Duration,ShouldStopAfterFirstCollision);
        Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem,&UARPGGameInstanceSubsystem::BindToMoveFinish);
        GameInstanceSubsystem->MoveRecords.Emplace(Record);
    }
}

void UARPGGameInstanceSubsystem::MoveActorTowardActor(AActor* Actor, AActor* Target,
                                                      FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                      float Duration)
{
    if (!Actor)
    {
        return;
    }

    if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
    {
        auto Record = UMoveActorTowardActorRecord::CreateRecord(Actor, Target, MoveFinishDelegate, MoveRate, Duration);
        Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem,&UARPGGameInstanceSubsystem::BindToMoveFinish);
        GameInstanceSubsystem->MoveRecords.Emplace(Record);
    }
}

void UARPGGameInstanceSubsystem::MoveActorTowardActorWithScale(AActor* Actor, AActor* Target,
                                                               FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                               float ScaleRate, float Duration)
{
    if (!Actor)
    {
        return;
    }

    if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
    {
        auto Record = UMoveActorTowardActorWithScaleRecord::CreateRecord(Actor, Target, MoveFinishDelegate, MoveRate, ScaleRate,
                                                               Duration);
        Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem,&UARPGGameInstanceSubsystem::BindToMoveFinish);
        GameInstanceSubsystem->MoveRecords.Emplace(Record);
    }
}

void UARPGGameInstanceSubsystem::MoveActorComplex(AActor* Actor, FTransformFunctionOfTime TransformFunctionOfTime, TArray<AActor*> IgnoreActors,
                                                  FCollisionDelegate OnCollision, float Duration, bool ShouldStopAfterFirstCollision)
{
    if (!Actor)
    {
        return;
    }

    if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
    {
        auto Record = UMoveActorComplex::CreateRecord(Actor, TransformFunctionOfTime,IgnoreActors, OnCollision, Duration,ShouldStopAfterFirstCollision);
        Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem,&UARPGGameInstanceSubsystem::BindToMoveFinish);
        GameInstanceSubsystem->MoveRecords.Emplace(Record);
    }
}

bool UARPGGameInstanceSubsystem::Tick(float DeltaSeconds)
{
    for (int i = MoveRecords.Num()-1; i >=0; --i)
    {
        MoveRecords[i]->Move(DeltaSeconds);
    }
    
    return true;
}


