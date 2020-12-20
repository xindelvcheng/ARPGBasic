// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCastAction.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "ARPGSpecialEffectsSubsystem.h"

AARPGCastAction::AARPGCastAction()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneComponent");
	SetRootComponent(DefaultSceneComponent);
}

void AARPGCastAction::BeginPlay()
{
	Super::BeginPlay();

	InitTaskObjects();
}

// Called every frame
void AARPGCastAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AARPGCastAction::OnActionActivate()
{
	verifyf(MeleeAttackMontages.Num()>0, TEXT("AARPGCastAction没有设置施法动作"));

	SetActorTickEnabled(true);
	Super::OnActionActivate();
}

void AARPGCastAction::OnActionFinished(AARPGAction* Action)
{
	Super::OnActionFinished(Action);

	SetActorTickEnabled(false);
}

void AARPGCastAction::InitTaskObjects()
{
	Tasks.Empty();
	for (const FSimpleTaskStruct ActionTaskStruct : ActionTaskStructs)
	{
		Tasks.Emplace(UARPGSimpleTask::Create(this, ActionTaskStruct,GetActorTransform()));
	}
}

AARPGCastAction* AARPGCastAction::CreateARPGCastAction(UObject* WorldContextObject,
                                                       TSubclassOf<AARPGCastAction> ARPGCastActionClass,
                                                       AARPGCharacter* ActionOwnerCharacter, int ActionExclusiveGroupID)
{
	if (AARPGCastAction* Action = CreateARPGAction<AARPGCastAction>(
		WorldContextObject, ARPGCastActionClass, ActionOwnerCharacter, ActionExclusiveGroupID))
	{
		Action->InitTaskObjects();
		return Action;
	};

	UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("CreateARPGCastAction出错"));
	return nullptr;
}

#if WITH_EDITOR
void AARPGCastAction::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(AARPGCastAction, ActionTaskStructs))
	{
		InitTaskObjects();
	}
}

#endif


UARPGSimpleTask* UARPGSimpleTask::Create(AARPGCastAction* TaskOwnerAction, FSimpleTaskStruct ActionTaskStruct,
                                                   FTransform TaskTransform)
{
	UARPGSimpleTask* Task = NewObject<UARPGSimpleTask>();
	Task->OwnerAction = TaskOwnerAction;
	Task->SpecialEffectCreatureClass = ActionTaskStruct.SpecialEffectCreatureClass;
	Task->Transform = ActionTaskStruct.LayoutDescription.GetAbsoluteTransform(TaskTransform.GetLocation());
	Task->StartTime = ActionTaskStruct.CreateEffectCreatureTime;
	Task->Duration = ActionTaskStruct.Duration;
	Task->EndTime = Task->StartTime + Task->Duration;

	FTimerHandle StartTimerHandle;
	TaskOwnerAction->GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([Task]()
	{
		Task->OnTaskExecuted();
	}), Task->StartTime, false);

	FTimerHandle EndTimerHandle;
	TaskOwnerAction->GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([Task]()
	{
		Task->OnTaskFinished();
	}), Task->EndTime, false);
	return Task;
}

void UARPGSimpleTask::OnTaskExecuted()
{
	Super::OnTaskExecuted();
	SpecialEffectCreature = AARPGSpecialEffectCreature::Create(SpecialEffectCreatureClass, Transform,
	                                                           OwnerAction->GetOwnerCharacter());
}

void UARPGSimpleTask::OnTaskFinished()
{
	Super::OnTaskFinished();

	if (SpecialEffectCreature)
	{
		SpecialEffectCreature->Destroy();
	}
}


UTask* UTask::Create(AARPGCastAction* TaskOwnerAction, float TaskStartTime, float TaskDuration,
                         FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished)
{
	if (UTask* Task = NewObject<UTask>(TaskOwnerAction))
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
