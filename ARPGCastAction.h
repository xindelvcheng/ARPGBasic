// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGAction.h"
#include "Engine/DataTable.h"

#include "ARPGCastAction.generated.h"

class AARPGSpecialEffectCreature;
class AARPGCastAction;
DECLARE_DYNAMIC_DELEGATE(FTaskDelegate);

UCLASS()
class UTask : public UObject
{
	GENERATED_BODY()

	FTaskDelegate OnTaskExecutedDelegate;
	FTaskDelegate OnTaskFinishedDelegate;
public:

	float StartTime = 0.1;
	float Duration = 0.5;
	float EndTime = 0.6;

	FTransform Transform;

	UPROPERTY()
	AARPGCastAction* OwnerAction;

	UFUNCTION(BlueprintCallable,Category="ARPGSpell",DisplayName="CreateARPGSpellTask")
	static UTask* Create(AARPGCastAction* TaskOwnerAction, float TaskStartTime, float TaskDuration,
	                         FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished);

	virtual void OnTaskExecuted()
	{
		OnTaskExecutedDelegate.ExecuteIfBound();
	};

	virtual void OnTaskFinished()
	{
		OnTaskFinishedDelegate.ExecuteIfBound();
	};
};

struct FSimpleTaskStruct;

UCLASS(BlueprintType)
class UARPGSimpleTask : public UTask
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AARPGSpecialEffectCreature> SpecialEffectCreatureClass;

	UPROPERTY()
	AARPGSpecialEffectCreature* SpecialEffectCreature;
public:

	static UARPGSimpleTask* Create(AARPGCastAction* TaskOwnerAction, FSimpleTaskStruct ActionTaskStruct,
	                                         FTransform RelativeTransform);

	virtual void OnTaskExecuted() override;
	virtual void OnTaskFinished() override;
};


UENUM(BlueprintType)
enum class ERotationTypeEnum:uint8
{
	NoRotation,
	CircleFaceInner,
	CircleFaceOuter
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
	FVector Scale = FVector{1, 1, 1};

	FVector GetRelativeLocation() const
	{
		return (FVector{RowsNumber, ColumnsNumber, LayersNumber} / 2 - FVector{
			RowIndex + 0.5f, ColumnIndex + 0.5f, LayerIndex + 0.5f
		}) * FVector{(Width / RowsNumber), -(Length / ColumnsNumber), (Height / LayersNumber)};
	}

	FRotator GetAbsoluteRotation() const
	{
		switch (RotationType)
		{
		case ERotationTypeEnum::NoRotation:
			return FRotator{};
		case ERotationTypeEnum::CircleFaceInner:
			return (-GetRelativeLocation()).Rotation();
		case ERotationTypeEnum::CircleFaceOuter:
			return GetRelativeLocation().Rotation();
		}
		return FRotator{};
	};

	FTransform GetAbsoluteTransform(FVector Origin) const
	{
		return FTransform{GetAbsoluteRotation(), Origin + GetRelativeLocation(), Scale};
	}
};

USTRUCT(BlueprintType)
struct FSimpleTaskStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float CreateEffectCreatureTime = 0.1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Duration = 0.5;

	float DestroyEffectCreatureTime = 0.6;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	TSubclassOf<AARPGSpecialEffectCreature> SpecialEffectCreatureClass;

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

/**
 * 
 */
UCLASS()
class AARPGCastAction : public AARPGMeleeAttackAction
{
	GENERATED_BODY()

	/*ARPGCastAction的位置是定向法术的原点*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
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


	virtual void OnActionActivate() override;
	virtual void OnActionFinished(AARPGAction* Action) override;

	void InitTaskObjects();

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static AARPGCastAction* CreateARPGCastAction(UObject* WorldContextObject, TSubclassOf<AARPGCastAction> ARPGCastActionClass,AARPGCharacter* ActionOwnerCharacter,
	                                             int ActionExclusiveGroupID);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
