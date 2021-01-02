// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCastAction.h"


#include "ARPGBasicSettings.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGCoreSubsystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ARPGSpecialEffectsSubsystem.h"
#include "ARPGStaticFunctions.h"

void AARPGCastAction::InitTasks()
{
	Tasks.Empty();

	float LastTaskEndTime = 0;
	for (const FSimpleTaskStruct ActionTaskStruct : ActionTaskStructs)
	{
		Tasks.Emplace(UARPGSimpleTask::Create(this, ActionTaskStruct, GetActorTransform()));
		LastTaskEndTime = std::max(
			LastTaskEndTime, ActionTaskStruct.CreateEffectCreatureTime + ActionTaskStruct.Duration);
	}

	if (bUseLastTaskEndTimeAsCastActionFinishTime)
	{
		Duration = LastTaskEndTime;
	}
}

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

	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("生成技能执行者%s失败，配置错误"), *SpellName.ToString()),15,FColor::Red);
	return nullptr;
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


UARPGSimpleTask* UARPGSimpleTask::Create(AARPGCastAction* TaskOwnerAction, FSimpleTaskStruct ActionTaskStruct,
                                         FTransform TaskTransform)
{
	UARPGSimpleTask* Task = UTask::Create<UARPGSimpleTask>(UARPGSimpleTask::StaticClass(), TaskOwnerAction,
	                                                       ActionTaskStruct.CreateEffectCreatureTime,
	                                                       ActionTaskStruct.Duration, FTaskDelegate{},
	                                                       FTaskDelegate{});
	Task->SpecialEffectCreatureClass = ActionTaskStruct.SpecialEffectCreatureClass;
	Task->LayoutDescription = ActionTaskStruct.LayoutDescription;

	return Task;
}

void UARPGSimpleTask::OnTaskExecuted()
{
	Super::OnTaskExecuted();

	OwnerAction->GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		Transform = LayoutDescription.CalculateAbsoluteTransform(OwnerAction->GetActorLocation(),OwnerAction->GetActorRotation());
		SpecialEffectCreature = AARPGSpecialEffectCreature::Create(SpecialEffectCreatureClass, Transform,
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

void UTask::OnTaskFinished()
{
	OnTaskFinishedDelegate.ExecuteIfBound();
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
