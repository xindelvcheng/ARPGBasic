// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGAction.h"
#include "ARPGGameInstanceSubsystem.h"
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

protected:
	virtual void OnTaskExecuted()
	{
		OnTaskExecutedDelegate.ExecuteIfBound();
	};

	virtual void OnTaskFinished();
public:

	float StartTime = 0.1;
	float Duration = 0.5;
	float EndTime = 0.6;

	FTimerHandle StartTimerHandle;
	FTimerHandle EndTimerHandle;

	FTransform Transform;

	UPROPERTY()
	AARPGCastAction* OwnerAction;

	template <typename T>
	static T* Create(TSubclassOf<UTask> TaskClass, AARPGCastAction* TaskOwnerAction, float TaskStartTime,
	                 float TaskDuration,
	                 FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished);

	UFUNCTION(BlueprintCallable,Category="SpellTask")
	void ExecutedTask();
	UFUNCTION(BlueprintCallable,Category="SpellTask")
	void FinishTask();
};

struct FSimpleTaskStruct;


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
	float Length = 1500;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Width = 1500;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float Height = 300;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float RowsNumber = 3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float ColumnsNumber = 3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float LayersNumber = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float RowIndex = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	float ColumnIndex = 1;

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

	FTransform CalculateAbsoluteTransform(FVector Origin) const
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

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	TSubclassOf<AARPGSpecialEffectCreature> SpecialEffectCreatureClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpell")
	FGridLayoutStruct LayoutDescription;
};

UENUM()
enum class ESpellTypeEnum:uint8
{
	Direction,
	Target
};

USTRUCT(BlueprintType)
struct FSimpleCastActionDescriptionStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpellDefinition")
	TArray<FSimpleTaskStruct> ActionTaskStructs;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpellDefinition")
	ESpellTypeEnum SpellTypeEnum;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpellDefinition")
	float MaxDistance = 1500;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	bool bUseLastTaskEndTimeAsCastActionFinishTime = true;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess,EditCondition=
        "!bUseLastTaskEndTimeAsCastActionFinishTime",EditConditionHides))
	float Duration = 1.5;
};

UCLASS(BlueprintType)
class UARPGSimpleTask : public UTask
{
	GENERATED_BODY()

	TSubclassOf<AARPGSpecialEffectCreature> SpecialEffectCreatureClass;

	UPROPERTY()
	AARPGSpecialEffectCreature* SpecialEffectCreature;
	
	FGridLayoutStruct LayoutDescription;
	
	virtual void OnTaskExecuted() override;
	virtual void OnTaskFinished() override;
public:
	float CreatureLifeDuration = 0.5;
	
	static UARPGSimpleTask* Create(AARPGCastAction* TaskOwnerAction, FSimpleTaskStruct ActionTaskStruct,
	                               FTransform RelativeTransform);
};


/*
 * 施法型技能
 */
UCLASS()
class AARPGCastAction : public AARPGMeleeAttackAction
{
	GENERATED_BODY()

	/*ARPGCastAction的位置是定向法术的原点*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	ESpellTypeEnum SpellTypeEnum;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	float MaxDistance;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	bool bUseLastTaskEndTimeAsCastActionFinishTime;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess,EditCondition=
		"!bUseLastTaskEndTimeAsCastActionFinishTime",EditConditionHides))
	float Duration = 1.5;


	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCastAction",meta=(AllowPrivateAccess))
	TArray<FSimpleTaskStruct> ActionTaskStructs;

	UPROPERTY()
	TArray<UTask*> Tasks;

	void InitTasks();

public:
	AARPGCastAction();


	virtual TArray<FSimpleTaskStruct>& GetActionTaskStructs()
	{
		return ActionTaskStructs;
	}

	virtual void SetActionTaskStructs(const TArray<FSimpleTaskStruct>& NewActionTaskStructs)
	{
		this->ActionTaskStructs = NewActionTaskStructs;
		InitTasks();
	}

	static AARPGCastAction* Create(AARPGCharacter* ActionOwnerCharacter,
	                               const FSimpleCastActionDescriptionStruct& CastActionDescription,
	                               TArray<UAnimMontage*> CastAnimMontages,
	                               FActionFinishDelegate ActionFinishedDelegate);

	static AARPGCastAction* Create(AARPGCharacter* ActionOwnerCharacter, const FName& SpellName,
	                               FActionFinishDelegate ActionFinishedDelegate);


	virtual float GetMaxDistance()
	{
		return MaxDistance;
	}

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;


	virtual void OnActionActivate() override;
	virtual void OnActionFinished(AARPGAction* Action) override;

	void StartAllTask();

	void StopAllTask();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
