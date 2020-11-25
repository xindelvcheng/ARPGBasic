// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "APRGGameSaver.h"
#include "CharacterStatusComponent.h"
#include "CoreMinimal.h"

#include "ARPGMainCharacter.h"
#include "ARPGPlayerController.h"

#include "ARPGStatusWidget.h"
#include "GameItem.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGGameInstanceSubsystem.generated.h"

class USaveGame;
DECLARE_DYNAMIC_DELEGATE(FMoveFinishDelegate);

UENUM()
enum class EChoice:uint8
{
    ChoiceA,
    ChoiceB
};


UCLASS()
class UActorMoveRecord : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
    AActor* Actor;

    float TimeLeft = FLT_MAX;

    FMoveFinishDelegate MoveFinishDelegate;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItemVisualEffect",meta=(AllowPrivateAccess))
    float ModifiedMoveRate = 1;

    float MoveRateModificationCoefficient = 1000;

    virtual void Move(float DeltaSeconds)
    {
    };

    virtual bool CheckContinueCondition()
    {
        if (TimeLeft > 0)
        {
            return true;
        }
        return false;
    };

    static UActorMoveRecord* CreateRecord(AActor* MoveActor, FMoveFinishDelegate& MoveFinishDelegate,
                                          float MoveRate, float Duration)
    {
        UActorMoveRecord* Record = NewObject<UActorMoveRecord>();
        InitializeRecord(Record, MoveActor, MoveFinishDelegate, MoveRate, Duration);
        return Record;
    };

protected:
    static void InitializeRecord(UActorMoveRecord* Record, AActor* MoveActor, FMoveFinishDelegate MoveFinishDelegate,
                                 float MoveRate, float Duration)
    {
        Record->Actor = MoveActor;
        Record->ModifiedMoveRate = MoveRate;
        Record->MoveFinishDelegate = MoveFinishDelegate;
        Record->TimeLeft = Duration;
    };
};


UCLASS()
class UMoveActorTowardsDirectionRecord : public UActorMoveRecord
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
    FVector Direction;

    static UActorMoveRecord* CreateRecord(AActor* MoveActor, FVector MoveDirection,
                                          FMoveFinishDelegate MoveFinishDelegate, float MoveRate, float Duration)
    {
        UMoveActorTowardsDirectionRecord* Record = NewObject<UMoveActorTowardsDirectionRecord>();
        InitializeRecord(Record, MoveActor, MoveDirection, MoveFinishDelegate, MoveRate, Duration);
        return Record;
    }

    virtual void Move(float DeltaSeconds) override
    {
        if (Actor)
        {
            Actor->AddActorWorldOffset(
                Direction.GetSafeNormal() * DeltaSeconds * ModifiedMoveRate * MoveRateModificationCoefficient);
        }
    }

protected:
    static void InitializeRecord(UMoveActorTowardsDirectionRecord* Record, AActor* MoveActor, FVector MoveDirection,
                                 FMoveFinishDelegate MoveFinishDelegate
                                 , float MoveRate, float Duration)
    {
        Super::InitializeRecord(Record, MoveActor, MoveFinishDelegate, MoveRate, Duration);
        Record->Direction = MoveDirection;
    };
};

UCLASS()
class UMoveActorTowardActorRecord : public UActorMoveRecord
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
    AActor* Target;

    static UActorMoveRecord* CreateRecord(AActor* MoveActor, AActor* TargetActor,
                                          FMoveFinishDelegate& MoveFinishDelegate, float MoveRate, float Duration)
    {
        UMoveActorTowardActorRecord* Record = NewObject<UMoveActorTowardActorRecord>();
        InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, Duration);
        return Record;
    }

    virtual void Move(float DeltaSeconds) override
    {
        if (Actor && Target)
        {
            const FVector Direction = Target->GetActorLocation() - Actor->GetActorLocation();
            Actor->AddActorWorldOffset(
                Direction.GetSafeNormal() * DeltaSeconds * ModifiedMoveRate * MoveRateModificationCoefficient);
        }
    }

protected:
    static void InitializeRecord(UMoveActorTowardActorRecord* Record, AActor* MoveActor, AActor* TargetActor,
                                 FMoveFinishDelegate MoveFinishDelegate,
                                 float MoveRate, float Duration)
    {
        Super::InitializeRecord(Record, MoveActor, MoveFinishDelegate, MoveRate, Duration);
        Record->Target = TargetActor;
    };
};

UCLASS()
class UMoveActorTowardActorWithScaleRecord : public UMoveActorTowardActorRecord
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItemVisualEffect",meta=(AllowPrivateAccess))
    float ModifiedScaleRate = 1;
    float ScaleRateModificationCoefficient = 0.0005;

    static UActorMoveRecord* CreateRecord(AActor* MoveActor, AActor* TargetActor,
                                          FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                          float ScaleRate, float Duration)
    {
        UMoveActorTowardActorWithScaleRecord* Record = NewObject<UMoveActorTowardActorWithScaleRecord>();
        InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, ScaleRate, Duration);
        return Record;
    }

    virtual void Move(float DeltaSeconds) override
    {
        Super::Move(DeltaSeconds);
        Actor->SetActorScale3D(
            Actor->GetActorScale3D() * pow(
                0.1, (1 / DeltaSeconds) * ModifiedScaleRate * ScaleRateModificationCoefficient));
    }

protected:
    static void InitializeRecord(UMoveActorTowardActorWithScaleRecord* Record, AActor* MoveActor, AActor* TargetActor,
                                 FMoveFinishDelegate MoveFinishDelegate,
                                 float MoveRate,
                                 float ScaleRate, float Duration)
    {
        Super::InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, Duration);
        Record->ModifiedScaleRate = ScaleRate;
    };
};


/**
 * 
 */
UCLASS()
class UARPGGameInstanceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    UPROPERTY()
    UAPRGArchiveManager* ArchiveManager;

    UPROPERTY()
    UAPRGGameSaver* GameSaver;

    UFUNCTION()
    void OnArchiveLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);

    UFUNCTION()
    void OnLevelLoaded();

    UPROPERTY()
    TArray<FArchiveInfoStruct> ArchiveInfos;

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;


public:
    FString ArchiveManageSlot = TEXT("ArchiveManageSlot");
    int UserIndexInt32;

    UFUNCTION(BlueprintCallable)
    TArray<FArchiveInfoStruct> GetArchiveInfos() const;

private:
    /*此三个变量在ARPG Player Controller的OnPosses回调中赋值，蓝图中可以获得*/
    TWeakObjectPtr<AARPGMainCharacter> MainCharacter;
    TWeakObjectPtr<AARPGPlayerController> MainCharacterController;
    TWeakObjectPtr<UARPGStatusWidget> StatusWidget;

public:

    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
    ))
    static AARPGMainCharacter* GetMainCharacter(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
    ))
    static AARPGPlayerController* GetMainCharacterController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
    ))
    static UARPGStatusWidget* GetMainCharacterStatusWidget(const UObject* WorldContextObject);

    void SetMainCharacter(AARPGMainCharacter* NewMainCharacter) { MainCharacter = NewMainCharacter; }

    void SetMainCharacterController(AARPGPlayerController* NewMainCharacterController)
    {
        MainCharacterController = NewMainCharacterController;
    }

    void SetMainCharacterStatusWidget(UARPGStatusWidget* NewARPGStatusWidget) { StatusWidget = NewARPGStatusWidget; }

    DECLARE_MULTICAST_DELEGATE(FSetupPlayerEvent)
    FSetupPlayerEvent OnPlayerSetupEnd;

    void SetupPlayer(AARPGMainCharacter* NewMainCharacter, AARPGPlayerController* NewMainCharacterController,
                     UARPGStatusWidget* NewARPGStatusWidget)
    {
        MainCharacter = NewMainCharacter;
        MainCharacterController = NewMainCharacterController;
        StatusWidget = NewARPGStatusWidget;
        OnPlayerSetupEnd.Broadcast();
    }

public:

    UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
    static void ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title, FText Content);

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
    FName CurrentStreamingLevelName = "";

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaveGameEvent);

    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameSaving;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameSaveSuccess;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameLoading;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameLoadSuccess;

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    bool SaveArchive(FString ArchiveName);


    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void LoadArchive(FString ArchiveName);


    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadLevelEvent);

    UPROPERTY(BlueprintAssignable)
    FLoadLevelEvent OnLevelLoading;
    UPROPERTY(BlueprintAssignable)
    FLoadLevelEvent OnLevelLoadSuccess;

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(ExpandEnumAsExecs="Choice"))
    static void RandomChoice(float ChanceA, EChoice& Choice);

    static void PrintLogToScreen(FString Message, float Time = 5, FColor Color = FColor::Yellow);

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC",BlueprintPure)
    static FTransform GetActorNearPositionTransform(AActor* OriginActor,
                                                    const FVector LocationOffset,
                                                    const FRotator RotationOffset);

    static UARPGGameInstanceSubsystem* Get(UWorld* World);

    UFUNCTION()
    bool Tick(float DeltaSeconds);

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(DefaultToSelf="Actor"))
    static void MoveActorTowardsDirection(AActor* Actor, FVector Direction,
                                          FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1,
                                          float Duration = 5);

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    static void MoveActorTowardActor(AActor* Actor, AActor* Target,
                                     FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1, float Duration = 5);

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    static void MoveActorTowardActorWithScale(AActor* Actor, AActor* Target,
                                              FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1,
                                              float ScaleRate = 1, float Duration = 5);

    UPROPERTY()
    TArray<UActorMoveRecord*> MoveRecords;
};
