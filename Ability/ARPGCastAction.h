// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGAction.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGSpellCreature.h"
#include "Engine/DataTable.h"

#include "ARPGCastAction.generated.h"

class AARPGCastAction;
DECLARE_DYNAMIC_DELEGATE(FTaskDelegate);

USTRUCT(BlueprintType)
struct FTaskDescriptionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpell")
	float CreateEffectCreatureTime = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpell")
	float Duration = 1.4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpell")
	TSubclassOf<AARPGSpellCreature> SpecialEffectCreatureClass;
};

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

	UFUNCTION(BlueprintCallable, Category="SpellTask")
	void ExecuteTask();
	UFUNCTION(BlueprintCallable, Category="SpellTask")
	void FinishTask();
};

struct FSimpleTaskDescriptionStruct;


UENUM(BlueprintType)
enum class ERotationTypeEnum:uint8
{
	NoRotation,
	CircleFaceInner,
	CircleFaceOuter
};



/**
 * 用于在ARPGCastAction中配置式创建ARPGSimpleTask
 */
USTRUCT(BlueprintType)
struct FSimpleTaskDescriptionStruct : public FTaskDescriptionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpell")
	FTransform LocalCoordinateTransform;
};

UCLASS(BlueprintType)
class UARPGSimpleTask : public UTask
{
	GENERATED_BODY()

	TSubclassOf<AARPGSpellCreature> SpecialEffectCreatureClass;

	UPROPERTY()
	AARPGSpellCreature* SpecialEffectCreature;

	FTransform LocalTransform;

	virtual void OnTaskExecuted() override;
	virtual void OnTaskFinished() override;
public:
	float CreatureLifeDuration = 0.5;

	static UARPGSimpleTask* Create(AARPGCastAction* TaskOwnerAction, FSimpleTaskDescriptionStruct ActionTaskStruct);
};


UENUM()
enum class ESpellTypeEnum:uint8
{
	Direction,
	Target
};

UENUM()
enum class ESpellCreatureClusterEnum :uint8 { Polygon, Radiation };

UENUM()
enum class ESpellCreatureDirectionEnum :uint8 { Outward, Inward, Forward };


/**
 * 用于在ARPGCastAction中配置式创建ClusterDescriptionTask
 */
USTRUCT()
struct FClusterTaskDescription : public FTaskDescriptionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="ARPGSpell Definition")
	ESpellCreatureClusterEnum SpellCreatureClusterEnum;

	UPROPERTY(EditAnywhere, Category="ARPGSpell Definition")
	ESpellCreatureDirectionEnum SpellCreatureDirectionEnum;

	/*半径*/
	UPROPERTY(EditAnywhere, Category="ARPGSpell Definition")
	float Radius = 100;

	/*弧度*/
	UPROPERTY(EditAnywhere, Category="ARPGSpell Definition",
		meta=(EditCondition="SpellCreatureClusterEnum==ESpellCreatureClusterEnum::Radiation", EditConditionHides))
	float Radian = PI / 6;

	UPROPERTY
	(EditAnywhere, Category="ARPGSpell Definition")
	int ElementsNum = 3;

	/*生成Creature的时间间隔，若不为0，Creature将逆时针依次产生，此时需注意调整Duration包含这些后续生成的Creature的存在时间*/
	UPROPERTY(EditAnywhere, Category="ClusterTask")
	float SpawnTimeInterval = 0;

	UPROPERTY(EditAnywhere, Category="ClusterTask",meta=(EditCondition="SpawnTimeInterval > 0", EditConditionHides))
	bool bAutoCalculateDuration = true;
};

UCLASS()
class UClusterTask : public UTask
{
	GENERATED_BODY()

	TSubclassOf<AARPGSpellCreature> SpecialEffectCreatureClass;

	UPROPERTY(EditAnywhere, Category="ClusterTask")
	ESpellCreatureClusterEnum SpellCreatureClusterEnum;

	UPROPERTY(EditAnywhere, Category="ClusterTask")
	ESpellCreatureDirectionEnum SpellCreatureDirectionEnum;

	/*半径*/
	UPROPERTY(EditAnywhere, Category="ClusterTask")
	float Radius = 100;

	/*弧度*/
	UPROPERTY(EditAnywhere, Category="ClusterTask",
		meta=(EditCondition="SpellCreatureClusterEnum==ESpellCreatureClusterEnum::Radiation", EditConditionHides))
	float Radian;

	UPROPERTY(EditAnywhere, Category="ClusterTask")
	int ElementsNum;

	int ElementIndex;

	UPROPERTY(EditAnywhere, Category="ClusterTask")
	float SpawnTimeInterval = 0;

	UPROPERTY()
	TArray<AARPGSpellCreature*> Creatures;

	TArray<FTimerHandle> TimerHandles;

	void SpawnCreatures();

	void SpawnNextCreature();
protected:
	virtual void OnTaskExecuted() override;
	virtual void OnTaskFinished() override;

public:
	static UClusterTask* Create(AARPGCastAction* TaskOwnerAction, FClusterTaskDescription ClusterTaskDescription);
};

/**
 * 用于生成配置式生成AARPGCastAction，不需要手动创建继承AARPGCastAction的蓝图资源
 */
USTRUCT(BlueprintType)
struct FSimpleCastActionDescriptionStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition")
	TArray<FSimpleTaskDescriptionStruct> ActionTaskStructs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	TArray<FClusterTaskDescription> ActionClusterTaskStructs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition")
	ESpellTypeEnum SpellTypeEnum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition")
	float SPCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition")
	float MaxDistance = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition")
	bool bUseLastTaskEndTimeAsCastActionFinishTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Definition", meta=(AllowPrivateAccess, EditCondition=
		"!bUseLastTaskEndTimeAsCastActionFinishTime", EditConditionHides))
	float Duration = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Information")
	FText ActionDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Information")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Information")
	FText Introduction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpell Information")
	FText DetailDescription;
};


/*
 * 施法型技能
 */
UCLASS()
class AARPGCastAction : public AARPGMultiMontageAction
{
	GENERATED_BODY()

	/*ARPGCastAction的位置是定向法术的伤害中心*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	USceneComponent* DefaultSceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	ESpellTypeEnum SpellTypeEnum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	float SPCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	float MaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	bool bUseLastTaskEndTimeAsCastActionFinishTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess, EditCondition=
		"!bUseLastTaskEndTimeAsCastActionFinishTime", EditConditionHides))
	float Duration = 1.5;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	TArray<FSimpleTaskDescriptionStruct> ActionSimpleTaskStructs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCastAction", meta=(AllowPrivateAccess))
	TArray<FClusterTaskDescription> ActionClusterTaskStructs;


	UPROPERTY()
	TArray<UTask*> Tasks;

	void InitTasks();

	void StartAllTask();

	void StopAllTask();

	virtual void Tick(float DeltaSeconds) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	AARPGCastAction();

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
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	virtual bool CheckActionActivateConditionAndPayCost() override;

	virtual void OnActionActivate() override;
	virtual void OnActionFinished(AARPGAction* Action) override;

	/*-Start- 施法动画仅是视觉特效 -Start-*/
	virtual void OnMontageBegin(UAnimMontage* Montage) override { ; }
	virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& PointPayload) override { ; }
	virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override { ; }
	/*-End- 施法动画仅是视觉特效 -End-*/

public:
	/*-Start- Getter & Setter -Start-*/

	virtual TArray<FSimpleTaskDescriptionStruct>& GetActionTaskStructs() { return ActionSimpleTaskStructs; }

	virtual void SetActionTaskStructs(const TArray<FSimpleTaskDescriptionStruct>& NewActionTaskStructs)
	{
		this->ActionSimpleTaskStructs = NewActionTaskStructs;
		InitTasks();
	}


	TArray<FClusterTaskDescription>& GetActionClusterTaskStructs() { return ActionClusterTaskStructs; }

	void SetActionClusterTaskStructs(const TArray<FClusterTaskDescription>& NewActionClusterTaskStructs)
	{
		this->ActionClusterTaskStructs = NewActionClusterTaskStructs;
		InitTasks();
	}

	/*-End- Getter & Setter -End-*/
};

class UARPGDamageBoxComponent;
