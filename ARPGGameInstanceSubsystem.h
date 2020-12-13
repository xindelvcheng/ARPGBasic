// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "APRGGameSaver.h"
#include "CharacterStatusComponent.h"
#include "CoreMinimal.h"

#include "ARPGMainCharacter.h"
#include "ARPGPlayerController.h"

#include "ARPGStatusWidget.h"
#include "GameItem.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGGameInstanceSubsystem.generated.h"

class USaveGame;
class UActorMoveRecord;

DECLARE_DYNAMIC_DELEGATE(FMoveFinishDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMoveFinishEvent, UActorMoveRecord*, Record);

DECLARE_DYNAMIC_DELEGATE_OneParam(FCollisionDelegate, AActor*, OtherActor);

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FTransform, FTransformFunctionOfTime, float, Time);

UENUM()
enum class EChoice:uint8
{
	ChoiceA,
	ChoiceB
};

UCLASS()
class UARPGGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	/*此三个变量在ARPG Player Controller的OnPosses回调中赋值，蓝图中可以获得*/
	TWeakObjectPtr<AARPGMainCharacter> MainCharacter;
	TWeakObjectPtr<AARPGPlayerController> MainCharacterController;
	TWeakObjectPtr<UARPGStatusWidget> StatusWidget;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY()
	UUserWidget* LoadingWidget;
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FPreLoadMapDelegate, const FString& /* MapName */);
	FPreLoadMapDelegate PreLoadMap;
	DECLARE_MULTICAST_DELEGATE_OneParam(FPostLoadMapDelegate, UWorld* /* LoadedWorld */);
	FPostLoadMapDelegate PostLoadMap;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPreLoadMapEvent,const FString&, MapName);
	UPROPERTY(BlueprintAssignable,Category="ARPGGameInstanceSubsystem")
	FPreLoadMapEvent OnLoadingMap;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapLoadedEvent,UWorld*, LoadedWorld);
	UPROPERTY(BlueprintAssignable,Category="ARPGGameInstanceSubsystem")
    FMapLoadedEvent OnMapLoaded;

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


	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static void ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title, FText Content);


	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(ExpandEnumAsExecs="Choice"))
	static void RandomChoice(float ChanceA, EChoice& Choice);

	static void PrintLogToScreen(FString Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(FText Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(float Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(UObject* Message, float Time = 5, FColor Color = FColor::Yellow);
	

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",BlueprintPure)
	static FTransform GetActorNearPositionTransform(AActor* OriginActor,
	                                                const FVector LocationOffset,
	                                                const FRotator RotationOffset);

	static UARPGGameInstanceSubsystem* Get(UWorld* World);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",BlueprintPure)
	static FVector2D GetScreenSize();

	UFUNCTION()
	bool Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(DefaultToSelf="Actor"))
	static void MoveActorTowardsDirection(AActor* Actor, FVector Direction,
	                                      FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1,
	                                      float Duration = 5);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(DefaultToSelf="Actor"))
	static void MoveActorTowardsDirectionFinishOnCollision(AActor* Actor, FVector Direction,
	                                                       TArray<AActor*> IgnoreActors,
	                                                       FCollisionDelegate OnCollision, float MoveRate = 1,
	                                                       float Duration = 5,
	                                                       bool ShouldStopAfterFirstCollision = true);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(DefaultToSelf="Actor"))
	static void MoveActorTowardDirectionFinishOnCollisionWithScale(AActor* Actor, FVector Direction,
	                                                               TArray<AActor*> IgnoreActors,
	                                                               FCollisionDelegate OnCollision, float MoveRate = 1,
	                                                               float ScaleRate = 1,
	                                                               float Duration = 5,
	                                                               bool ShouldStopAfterFirstCollision = true);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	static void MoveActorTowardActor(AActor* Actor, AActor* Target,
	                                 FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1, float Duration = 5,
	                                 float AcceptableRadius = 20);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	static void MoveActorTowardActorWithScale(AActor* Actor, AActor* Target,
	                                          FMoveFinishDelegate MoveFinishDelegate, float MoveRate = 1,
	                                          float ScaleRate = 1, float Duration = 5, float AcceptableRadius = 20);
	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(DefaultToSelf="Actor"))
	static void MoveActorComplex(AActor* Actor, FTransformFunctionOfTime TransformFunctionOfTime,
	                             TArray<AActor*> IgnoreActors,
	                             FCollisionDelegate OnCollision, float Duration = 5,
	                             bool ShouldStopAfterFirstCollision = true);

	UPROPERTY()
	TArray<UActorMoveRecord*> MoveRecords;

	UFUNCTION()
	void BindToMoveFinish(UActorMoveRecord* Record) { MoveRecords.Remove(Record); }


	template <typename T>
	static T* SpawnActor(FTransform Transform, AARPGCharacter* OwnerCharacter)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Instigator = OwnerCharacter;
		ActorSpawnParameters.Owner = OwnerCharacter;
		ActorSpawnParameters.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		return OwnerCharacter->GetWorld()->SpawnActor<T>(Transform, ActorSpawnParameters);
	}
	
};


UCLASS()
class UActorMoveRecord : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
	TWeakObjectPtr<AActor> Actor;

	float TimeLeft = FLT_MAX;
	float TimePassed = 0;

	FTransformFunctionOfTime TransformFunction;
	FMoveFinishDelegate MoveFinishDelegate;
	FMoveFinishEvent MoveFinishEvent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItemVisualEffect",meta=(AllowPrivateAccess))
	float ModifiedMoveRate = 1;

	float MoveRateModificationCoefficient = 1000;

	virtual void Move(float DeltaSeconds)
	{
		if (CheckContinueCondition())
		{
			this->TimeLeft -= DeltaSeconds;
			this->TimePassed += DeltaSeconds;
			if (TransformFunction.IsBound())
			{
				Actor->SetActorTransform(TransformFunction.Execute(TimePassed));
			}
		}
		else
		{
			MoveFinishEvent.Broadcast(this);
		}
	};

	virtual bool CheckContinueCondition()
	{
		return TimeLeft > 0;
	};

	static UActorMoveRecord* CreateRecord(AActor* MoveActor, FMoveFinishDelegate MoveFinishDelegate,
	                                      float MoveRate, float Duration)
	{
		UActorMoveRecord* Record = NewObject<UActorMoveRecord>();
		InitializeRecord(Record, MoveActor, MoveFinishDelegate, MoveRate, Duration);
		return Record;
	};

	static UActorMoveRecord* CreateRecord(AActor* MoveActor, FTransformFunctionOfTime TransformFunctionOfTime,
	                                      FMoveFinishDelegate MoveFinishDelegate,
	                                      float Duration)
	{
		UActorMoveRecord* Record = NewObject<UActorMoveRecord>();
		InitializeRecord(Record, MoveActor, MoveFinishDelegate, 0, Duration);
		Record->TransformFunction = TransformFunctionOfTime;
		return Record;
	};

protected:
	static void InitializeRecord(UActorMoveRecord* Record, AActor* MoveActor, FMoveFinishDelegate MoveFinishDelegate,
	                             float MoveRate, float Duration)
	{
		Record->Actor = MoveActor;
		Record->ModifiedMoveRate = MoveRate;
		Record->MoveFinishEvent.Add(MoveFinishDelegate);
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
		Super::Move(DeltaSeconds);
		if (Actor.IsValid())
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
	float AcceptableRadius = 20;
	float CurrentDistance = FLT_MAX;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
	AActor* Target;

	static UActorMoveRecord* CreateRecord(AActor* MoveActor, AActor* TargetActor,
	                                      FMoveFinishDelegate& MoveFinishDelegate, float MoveRate, float Duration,
	                                      float AcceptableFinishRadius)
	{
		UMoveActorTowardActorRecord* Record = NewObject<UMoveActorTowardActorRecord>();
		InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, Duration,
		                 AcceptableFinishRadius);
		return Record;
	}

	virtual void Move(float DeltaSeconds) override
	{
		Super::Move(DeltaSeconds);
		if (Actor.IsValid() && Target)
		{
			const FVector Direction = Target->GetActorLocation() - Actor->GetActorLocation();
			CurrentDistance = Direction.Size();
			Actor->AddActorWorldOffset(
				Direction.GetSafeNormal() * DeltaSeconds * ModifiedMoveRate * MoveRateModificationCoefficient);
		}
	}

	virtual bool CheckContinueCondition() override
	{
		return TimeLeft > 0 && CurrentDistance > AcceptableRadius;
	}

protected:
	static void InitializeRecord(UMoveActorTowardActorRecord* Record, AActor* MoveActor, AActor* TargetActor,
	                             FMoveFinishDelegate MoveFinishDelegate,
	                             float MoveRate, float Duration, float AcceptableFinishRadius)
	{
		Super::InitializeRecord(Record, MoveActor, MoveFinishDelegate, MoveRate, Duration);
		Record->AcceptableRadius = AcceptableFinishRadius;
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
	static float GetScaleRateModificationCoefficient() { return 0.0005; }

	static UActorMoveRecord* CreateRecord(AActor* MoveActor, AActor* TargetActor,
	                                      FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
	                                      float ScaleRate, float Duration, float AcceptableFinishRadius)
	{
		UMoveActorTowardActorWithScaleRecord* Record = NewObject<UMoveActorTowardActorWithScaleRecord>();
		InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, ScaleRate, Duration,
		                 AcceptableFinishRadius);
		return Record;
	}

	static float CalculateScaleFactorInDeltaTime(float DeltaSeconds, float ScaleRate = 1, float EndScale = 0.1)
	{
		return pow(EndScale, (1 / DeltaSeconds) * ScaleRate * GetScaleRateModificationCoefficient());
	}

	virtual void Move(float DeltaSeconds) override
	{
		Super::Move(DeltaSeconds);
		if (Actor.IsValid())
		{
			Actor->SetActorScale3D(
				Actor->GetActorScale3D() * CalculateScaleFactorInDeltaTime(DeltaSeconds, ModifiedScaleRate));
		}
	}

protected:
	static void InitializeRecord(UMoveActorTowardActorWithScaleRecord* Record, AActor* MoveActor, AActor* TargetActor,
	                             FMoveFinishDelegate MoveFinishDelegate,
	                             float MoveRate,
	                             float ScaleRate, float Duration, float AcceptableFinishRadius)
	{
		Super::InitializeRecord(Record, MoveActor, TargetActor, MoveFinishDelegate, MoveRate, Duration,
		                        AcceptableFinishRadius);
		Record->ModifiedScaleRate = ScaleRate;
	};
};


UCLASS()
class UMoveActorTowardDirectionFinishOnCollision : public UMoveActorTowardsDirectionRecord
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

	bool bStopAfterCollision;
public:


	FCollisionDelegate CollisionsDelegate;


	static UActorMoveRecord* CreateRecord(AActor* MoveActor, FVector MoveDirection, TArray<AActor*> IgnoreActors,
	                                      FCollisionDelegate OnCollision, float MoveRate, float Duration,
	                                      bool ShouldDestroyAfterFirstCollision)
	{
		auto Record = NewObject<UMoveActorTowardDirectionFinishOnCollision>();
		InitializeRecord(Record, MoveActor, MoveDirection, IgnoreActors, OnCollision, MoveRate, Duration,
		                 ShouldDestroyAfterFirstCollision);
		return Record;
	}

	static void InitializeRecord(UMoveActorTowardDirectionFinishOnCollision* Record, AActor* MoveActor,
	                             FVector MoveDirection, TArray<AActor*> IgnoreActors,
	                             FCollisionDelegate OnCollision, float MoveRate, float Duration,
	                             bool ShouldDestroyAfterFirstCollision)
	{
		Super::InitializeRecord(Record, MoveActor, MoveDirection, FMoveFinishDelegate{}, MoveRate, Duration);
		Record->IgnoreActors = IgnoreActors;
		Record->CollisionsDelegate = OnCollision;
		Record->bStopAfterCollision = ShouldDestroyAfterFirstCollision;
		MoveActor->OnActorBeginOverlap.AddDynamic(
			Record, &UMoveActorTowardDirectionFinishOnCollision::BindToActorBeginOverlap);
	}

	UFUNCTION()
	void BindToActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
	{
		if (OverlappedActor->GetOwner() == OtherActor->GetOwner()
			|| OverlappedActor->GetInstigator() == OtherActor->GetInstigator()
			|| Cast<AGameItem>(OtherActor)
			|| OtherActor->GetClass() == OverlappedActor->GetClass()
			|| IgnoreActors.Contains(OtherActor))
		{
			return;
		}
		CollisionsDelegate.ExecuteIfBound(OtherActor);

		if (bStopAfterCollision)
		{
			MoveFinishEvent.Broadcast(this);
		}
	}
};

UCLASS()
class UMoveActorTowardDirectionFinishOnCollisionWithScale : public UMoveActorTowardDirectionFinishOnCollision
{
	GENERATED_BODY()

	float ModifiedScaleRate = 1;
public:
	static UActorMoveRecord* CreateRecord(AActor* MoveActor, FVector MoveDirection, TArray<AActor*> IgnoreActors,
	                                      FCollisionDelegate OnCollision, float MoveRate, float ScaleRate,
	                                      float Duration,
	                                      bool ShouldDestroyAfterFirstCollision)
	{
		auto Record = NewObject<UMoveActorTowardDirectionFinishOnCollisionWithScale>();
		InitializeRecord(Record, MoveActor, MoveDirection, IgnoreActors, OnCollision, MoveRate, ScaleRate, Duration,
		                 ShouldDestroyAfterFirstCollision);
		return Record;
	}

	static void InitializeRecord(UMoveActorTowardDirectionFinishOnCollisionWithScale* Record, AActor* MoveActor,
	                             FVector MoveDirection, TArray<AActor*> IgnoreActors,
	                             FCollisionDelegate OnCollision, float MoveRate, float ScaleRate, float Duration,
	                             bool ShouldDestroyAfterFirstCollision)
	{
		Super::InitializeRecord(Record, MoveActor, MoveDirection, IgnoreActors, OnCollision, MoveRate, Duration,
		                        ShouldDestroyAfterFirstCollision);
		Record->ModifiedScaleRate = ScaleRate;
	}

	virtual void Move(float DeltaSeconds) override
	{
		Super::Move(DeltaSeconds);
		if (Actor.IsValid())
		{
			Actor->SetActorScale3D(
				Actor->GetActorScale3D() * UMoveActorTowardActorWithScaleRecord::CalculateScaleFactorInDeltaTime(
					DeltaSeconds, ModifiedScaleRate));
		}
	}
};

UCLASS()
class UMoveActorComplex : public UMoveActorTowardDirectionFinishOnCollision
{
	GENERATED_BODY()
public:
	virtual void Move(float DeltaSeconds) override
	{
		Super::Move(DeltaSeconds);
	};

	static UActorMoveRecord* CreateRecord(AActor* MoveActor, FTransformFunctionOfTime TransformFunctionOfTime,
	                                      TArray<AActor*> IgnoreActors,
	                                      FCollisionDelegate OnCollision, float Duration,
	                                      bool ShouldDestroyAfterFirstCollision)
	{
		auto Record = NewObject<UMoveActorComplex>();
		Super::InitializeRecord(Record, MoveActor, {0, 0, 0}, IgnoreActors,
		                        OnCollision, 0, Duration,
		                        ShouldDestroyAfterFirstCollision);
		Record->TransformFunction = TransformFunctionOfTime;
		return Record;
	}
};

/**
 * 
 */
