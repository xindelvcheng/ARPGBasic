// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGAction.h"
#include "Engine/DataTable.h"

#include "ARPGCastAction.generated.h"

struct FSimpleTaskStruct;
class UTask;
struct FGridLayoutStruct;



DECLARE_DYNAMIC_DELEGATE(FTaskDelegate);

/**
 * 
 */
UCLASS()
class AARPGCastAction : public AARPGAction
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCastAction")
	USceneComponent* DefaultSceneComponent;

public:
	AARPGCastAction();

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpell")
	TArray<UTask*> Tasks;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpell")
	TArray<FSimpleTaskStruct> ActionTaskStructs;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	void InitTaskObjects();

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static AARPGCastAction* CreateARPGCastAction(UObject* WorldContextObject, FName AbilityName,
	                                             AARPGCharacter* ActionOwnerCharacter, int ActionExclusiveGroupID);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};

UCLASS()
class UTask : public UObject
{
	GENERATED_BODY()

	FTaskDelegate OnTaskExecutedDelegate;
	FTaskDelegate OnTaskFinishedDelegate;
public:
	float StartTime;
	float TimeRemaining;
	float TimeElapsed;

	FTransform Transform;

	UPROPERTY()
	AARPGCastAction* OwnerAction;

	UFUNCTION(BlueprintCallable,Category="ARPGSpell")
	static UTask* CreateTask(AARPGCastAction* TaskOwnerAction, float TaskStartTime, float TaskDuration,
	                         FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished);

	virtual void Tick(float DeltaTime)
	{
		TimeRemaining -= DeltaTime;
		TimeElapsed += DeltaTime;
	};

	virtual void OnTaskExecuted()
	{
		OnTaskExecutedDelegate.ExecuteIfBound();
	};

	virtual void OnTaskFinished()
	{
		OnTaskFinishedDelegate.ExecuteIfBound();
	};
};

UCLASS(BlueprintType)
class UARPGSimpleTask : public UTask
{
	GENERATED_BODY()

	UPROPERTY()
	UParticleSystem* VisualEffect;
	UPROPERTY()
	USoundBase* SoundEffect;
public:

	UFUNCTION(BlueprintCallable,Category="ARPGSpell")
	static UARPGSimpleTask* CreateSimpleTask(AARPGCastAction* TaskOwnerAction, float TaskStartTime, float TaskDuration,
	                                         UParticleSystem* NewVisualEffect, USoundBase* NewSoundEffect,
	                                         FTransform RelativeTransform);

	virtual void OnTaskExecuted() override;
	virtual void OnTaskFinished() override;
};



UENUM(BlueprintType)
enum class ERotationTypeEnum:uint8
{
	NoRotation,CircleFaceInner,CircleFaceOuter
};

USTRUCT(BlueprintType)
struct FGridLayoutStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Length = 300;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Width = 300;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Height = 300;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float RowsNumber = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float ColumnsNumber = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float LayersNumber = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float RowIndex = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float ColumnIndex = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float LayerIndex = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	ERotationTypeEnum RotationType = ERotationTypeEnum::NoRotation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	FVector Scale = FVector{1,1,1};

	FVector GetRelativeLocation() const
	{
		return (FVector{RowsNumber, ColumnsNumber, LayersNumber} / 2 - FVector{
            RowIndex + 0.5f, ColumnIndex + 0.5f, LayerIndex + 0.5f
        }) * FVector{(Width / RowsNumber),-(Length / ColumnsNumber),(Height / LayersNumber)};
	}

	FRotator GetAbsoluteRotation()const
	{
		switch (RotationType)
		{
		case ERotationTypeEnum::NoRotation:
			return FRotator{};
		case ERotationTypeEnum::CircleFaceInner:
			return (-GetRelativeLocation()).Rotation();
		case ERotationTypeEnum::CircleFaceOuter:
			return  GetRelativeLocation().Rotation();
		}
		return FRotator{};
	};

	FTransform GetAbsoluteTransform(FVector Origin) const
	{
		return FTransform{GetAbsoluteRotation(),Origin+GetRelativeLocation(),Scale};
	}
};

USTRUCT(BlueprintType)
struct FSimpleTaskStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float StartTime = 0.1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Duration = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	UParticleSystem* VisualEffect = nullptr;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	USoundBase* SoundEffect = nullptr;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	FGridLayoutStruct LayoutDescription;
};

USTRUCT(BlueprintType)
struct FSimpleTaskDataTableLine : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	TArray<FSimpleTaskStruct> SpellTasks;
	
};
