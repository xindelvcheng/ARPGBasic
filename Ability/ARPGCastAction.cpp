// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCastAction.h"


#include "ARPGBasicSettings.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGDamageBoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ARPGSpecialEffectsSubsystem.h"
#include "ARPGStaticFunctions.h"

template <typename T>
T* UTask::Create(TSubclassOf<UTask> TaskClass, AARPGCastAction* TaskOwnerAction, float TaskStartTime,
                 float TaskDuration,
                 FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished)
{
	if (T* Task = NewObject<T>(TaskOwnerAction, TaskClass))
	{
		Task->StartTime = TaskStartTime;
		Task->Duration = TaskDuration;
		Task->EndTime = TaskStartTime + TaskDuration;
		Task->OnTaskExecutedDelegate = TaskOnTaskExecuted;
		Task->OnTaskFinishedDelegate = TaskOnTaskFinished;
		Task->OwnerAction = TaskOwnerAction;
		return Task;
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("生成UTask出错"));
	}
	return nullptr;
}

void UTask::ExecuteTask()
{
	OnTaskExecuted();
}

void UTask::FinishTask()
{
	OwnerAction->GetWorldTimerManager().ClearTimer(StartTimerHandle);
	OwnerAction->GetWorldTimerManager().ClearTimer(EndTimerHandle);
	OnTaskFinished();
}

void UTask::OnTaskFinished()
{
	OnTaskFinishedDelegate.ExecuteIfBound();
}

UARPGSimpleTask* UARPGSimpleTask::Create(AARPGCastAction* TaskOwnerAction,
                                         FSimpleTaskDescriptionStruct ActionTaskStruct)
{
	UARPGSimpleTask* Task = UTask::Create<UARPGSimpleTask>(StaticClass(), TaskOwnerAction,
	                                                       ActionTaskStruct.CreateEffectCreatureTime,
	                                                       ActionTaskStruct.Duration, FTaskDelegate{},
	                                                       FTaskDelegate{});
	Task->SpecialEffectCreatureClass = ActionTaskStruct.SpecialEffectCreatureClass;
	Task->LocalTransform = ActionTaskStruct.LocalCoordinateTransform;

	return Task;
}

void UARPGSimpleTask::OnTaskExecuted()
{
	Super::OnTaskExecuted();

	OwnerAction->GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		Transform = UARPGStaticFunctions::ConvActorLocalTransformToWorldTransform(OwnerAction, LocalTransform);
		SpecialEffectCreature = AARPGSpellCreature::Create(SpecialEffectCreatureClass, Transform,
		                                                   OwnerAction->GetOwnerCharacter());
	}), StartTime, false);

	OwnerAction->GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		FinishTask();
	}), EndTime, false);
}

void UARPGSimpleTask::OnTaskFinished()
{
	Super::OnTaskFinished();

	if (SpecialEffectCreature)
	{
		SpecialEffectCreature->Destroy();
	}
}


UClusterTask* UClusterTask::Create(AARPGCastAction* TaskOwnerAction, FClusterTaskDescription ClusterTaskDescription)
{
	if (ClusterTaskDescription.SpawnTimeInterval > 0 && ClusterTaskDescription.bAutoCalculateDuration)
	{
		ClusterTaskDescription.Duration = ClusterTaskDescription.Duration + ClusterTaskDescription.SpawnTimeInterval *
			(ClusterTaskDescription.ElementsNum - 1);
	}

	UClusterTask* Task = UTask::Create<UClusterTask>(StaticClass(), TaskOwnerAction,
	                                                 ClusterTaskDescription.CreateEffectCreatureTime,
	                                                 ClusterTaskDescription.Duration, FTaskDelegate{},
	                                                 FTaskDelegate{});
	Task->SpecialEffectCreatureClass = ClusterTaskDescription.SpecialEffectCreatureClass;
	Task->SpellCreatureClusterEnum = ClusterTaskDescription.SpellCreatureClusterEnum;
	Task->SpellCreatureDirectionEnum = ClusterTaskDescription.SpellCreatureDirectionEnum;
	Task->Radius = ClusterTaskDescription.Radius;
	Task->Radian = ClusterTaskDescription.Radian;
	Task->ElementsNum = ClusterTaskDescription.ElementsNum;
	Task->SpawnTimeInterval = ClusterTaskDescription.SpawnTimeInterval;

	return Task;
}

void UClusterTask::OnTaskExecuted()
{
	Super::OnTaskExecuted();

	OwnerAction->GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		SpawnCreatures();
	}), StartTime, false);

	OwnerAction->GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		FinishTask();
	}), EndTime, false);
}


void UClusterTask::SpawnCreatures()
{
	for (int i = 0; i < ElementsNum; ++i)
	{
		if (SpawnTimeInterval > 0)
		{
			FTimerHandle TimerHandle;
			OwnerAction->GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				SpawnNextCreature();
			}), SpawnTimeInterval * i, false);
			TimerHandles.Add(TimerHandle);
		}
		else
		{
			SpawnNextCreature();
		}
	}
}

void UClusterTask::SpawnNextCreature()
{
	FTransform CreatureTransform;

	/*Calculate creatures location*/
	switch (SpellCreatureClusterEnum)
	{
	case ESpellCreatureClusterEnum::Polygon:
		{
			const float Theta = 2 * PI * ElementIndex / ElementsNum;
			CreatureTransform = UARPGStaticFunctions::ConvActorLocalTransformToWorldTransform(
				OwnerAction, FVector{Radius * cosf(Theta), Radius * sinf(Theta), 0});


			break;
		}

	case ESpellCreatureClusterEnum::Radiation:
		{
			const float Theta = -Radian / 2 + Radian * ElementIndex / (ElementsNum - 1);
			CreatureTransform = UARPGStaticFunctions::ConvActorLocalTransformToWorldTransform(
				OwnerAction, FVector{Radius * cosf(Theta), Radius * sinf(Theta), 0});
			CreatureTransform.SetRotation(
				(CreatureTransform.GetLocation() - OwnerAction->GetActorLocation()).ToOrientationQuat());
			break;
		}
	}

	/*Calculate creatures rotation*/
	switch (SpellCreatureDirectionEnum)
	{
	case ESpellCreatureDirectionEnum::Outward:
		{
			CreatureTransform.SetRotation(
				(CreatureTransform.GetLocation() - OwnerAction->GetActorLocation()).ToOrientationQuat());
			break;
		}
	case ESpellCreatureDirectionEnum::Inward:
		{
			CreatureTransform.SetRotation(
				(OwnerAction->GetActorLocation() - CreatureTransform.GetLocation()).ToOrientationQuat());
			break;
		}
	case ESpellCreatureDirectionEnum::Forward:
		{
			CreatureTransform.SetRotation(OwnerAction->GetActorRotation().Quaternion());
			break;
		}
	}

	/*creatures scale decided by OwnerAction*/
	CreatureTransform.SetScale3D(OwnerAction->GetActorScale3D());

	Creatures.Add(AARPGSpellCreature::Create(SpecialEffectCreatureClass, CreatureTransform,
	                                         OwnerAction->GetOwnerCharacter()));
	ElementIndex ++;
}

void UClusterTask::OnTaskFinished()
{
	Super::OnTaskFinished();

	for (AARPGSpellCreature* SpellCreature : Creatures)
	{
		if (SpellCreature)
		{
			SpellCreature->Destroy();
		}
	}

	for (FTimerHandle TimerHandle : TimerHandles)
	{
		OwnerAction->GetWorldTimerManager().ClearTimer(TimerHandle);
	}

	Creatures.Empty();
}

#pragma optimize("",off)
void AARPGCastAction::InitTasks()
{
	Tasks.Empty();

	float LastTaskEndTime = 0;

	/*Init Simple Tasks*/
	for (const FSimpleTaskDescriptionStruct& ActionTaskStruct : ActionSimpleTaskStructs)
	{
		UARPGSimpleTask* Task = UARPGSimpleTask::Create(this, ActionTaskStruct);
		Tasks.Emplace(Task);
		LastTaskEndTime = std::max(LastTaskEndTime, Task->EndTime);
	}

	/*Init Cluster Tasks*/
	for (const FClusterTaskDescription& ActionClusterTaskStruct : ActionClusterTaskStructs)
	{
		UClusterTask* Task = UClusterTask::Create(this, ActionClusterTaskStruct);
		Tasks.Emplace(Task);
		LastTaskEndTime = std::max(LastTaskEndTime, Task->EndTime);
	}

	/*如果bUseLastTaskEndTimeAsCastActionFinishTime=true，设置Action结束时间为最后一个Task的结束时间*/
	if (bUseLastTaskEndTimeAsCastActionFinishTime) { Duration = LastTaskEndTime; }
}
#pragma optimize("",on)

AARPGCastAction::AARPGCastAction()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneComponent");
	SetRootComponent(DefaultSceneComponent);
}

AARPGCastAction* AARPGCastAction::Create(AARPGCharacter* ActionOwnerCharacter,
                                         const FSimpleCastActionDescriptionStruct& CastActionDescription,
                                         TArray<UAnimMontage*> CastAnimMontages,
                                         FActionFinishDelegate ActionFinishedDelegate)
{
	if (AARPGCastAction* Action = ActionOwnerCharacter->GetWorld()->SpawnActorDeferred<AARPGCastAction>(
		StaticClass(), FTransform{}, ActionOwnerCharacter, ActionOwnerCharacter,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		Action->OnActionFinishedEvent().Add(ActionFinishedDelegate);
		Action->bUseLastTaskEndTimeAsCastActionFinishTime = CastActionDescription.
			bUseLastTaskEndTimeAsCastActionFinishTime;
		if (!Action->bUseLastTaskEndTimeAsCastActionFinishTime)
		{
			Action->Duration = CastActionDescription.Duration;
		}
		Action->SetActionTaskStructs(CastActionDescription.ActionTaskStructs);
		Action->SpellTypeEnum = CastActionDescription.SpellTypeEnum;
		Action->SPCost = CastActionDescription.SPCost;
		Action->MaxDistance = CastActionDescription.MaxDistance;
		Action->ActionMontages = CastAnimMontages;

		Action->SetItemDisplayName(CastActionDescription.ActionDisplayName);
		Action->SetItemIcon(CastActionDescription.ItemIcon);
		Action->SetItemIntroduction(CastActionDescription.Introduction);

		Action->FinishSpawning(FTransform{});
		return Action;
	};
	UARPGStaticFunctions::PrintLogToScreen(
		FString::Printf(TEXT("%s生成AARPGCastAction出现错误"), *ActionOwnerCharacter->GetName()));
	return nullptr;
}

AARPGCastAction* AARPGCastAction::Create(AARPGCharacter* ActionOwnerCharacter, const FName& SpellName,
                                         FActionFinishDelegate ActionFinishedDelegate)
{
	if (UARPGConfigSubsystem* ConfigSubsystem = UARPGConfigSubsystem::Get(ActionOwnerCharacter->GetWorld()))
	{
		if (UDataTable* DataTable = ConfigSubsystem->AbilityConfigDataTable)
		{
			if (const auto SpellDescription = DataTable->FindRow<FSimpleCastActionDescriptionStruct>(
				SpellName,TEXT("QuerySpell")))
			{
				if (AARPGMeleeAttackAction* MeleeAction = Cast<AARPGMeleeAttackAction>(
					ActionOwnerCharacter->GetCharacterCombatComponent()->GetCurrentMeleeAttackCollection()))
				{
					if (AARPGCastAction* CastAction = Create(ActionOwnerCharacter, *SpellDescription,
					                                         MeleeAction->MeleeAttackMontages,
					                                         ActionFinishedDelegate))
					{
						return CastAction;
					}
				}
			}
		}
	}

	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("生成技能执行者%s失败，配置错误"), *SpellName.ToString()), 15,
	                                       FColor::Red);
	return nullptr;
}

void AARPGCastAction::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitTasks();
}

void AARPGCastAction::BeginPlay()
{
	Super::BeginPlay();
}

bool AARPGCastAction::CheckActionActivateConditionAndPayCost()
{
	if (GetOwnerCharacter() && GetOwnerCharacter()->GetCurrentSP() > SPCost)
	{
		GetOwnerCharacter()->UpdateCurrentSP(-SPCost);
		return true;
	}
	return false;
}

// Called every frame
void AARPGCastAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARPGCastAction::OnActionActivate()
{
	verifyf(ActionMontages.Num()>0, TEXT("AARPGCastAction没有设置施法动作"));
	StartAllTask();

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		FinishAction();
	}), Duration, false);
	Super::OnActionActivate();
}

void AARPGCastAction::OnActionFinished(AARPGAction* Action)
{
	Super::OnActionFinished(Action);
	StopAllTask();

	SetActorTickEnabled(false);
}

void AARPGCastAction::StartAllTask()
{
	for (UTask* Task : Tasks)
	{
		Task->ExecuteTask();
	}
}

void AARPGCastAction::StopAllTask()
{
	for (UTask* Task : Tasks)
	{
		Task->FinishTask();
	}
}

#if WITH_EDITOR
void AARPGCastAction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
