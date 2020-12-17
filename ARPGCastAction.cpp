// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCastAction.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Particles/ParticleSystemComponent.h"

AARPGCastAction::AARPGCastAction()
{
	PrimaryActorTick.bCanEverTick = true;
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
	if (Tasks.Num() == 0)
	{
		return;
	}

	for (int i = Tasks.Num() - 1; i >= 0; --i)
	{
		UTask* Task = Tasks[i];
		if (Task->TimeElapsed >= Task->StartTime)
		{
			Task->Tick(DeltaTime);
			if (Task->TimeRemaining <= 0)
			{
				Tasks.RemoveAt(i);
			}
		}
	}
}

void AARPGCastAction::InitTaskObjects()
{
	Tasks.Empty();
	for (FSimpleTaskStruct ActionTaskStruct : ActionTaskStructs)
	{
		Tasks.Emplace(UARPGSimpleTask::CreateSimpleTask(this, ActionTaskStruct.StartTime, ActionTaskStruct.Duration,
		                                                ActionTaskStruct.SpecialEffectCreatureClass,
		                                                ActionTaskStruct.LayoutDescription.GetAbsoluteTransform(
			                                                this->GetActorLocation())));
	}
}

AARPGCastAction* AARPGCastAction::CreateARPGCastAction(UObject* WorldContextObject, FName AbilityName,
                                                       AARPGCharacter* ActionOwnerCharacter, int ActionExclusiveGroupID)
{
	if (UARPGConfigSubsystem* ConfigSubsystem = UARPGConfigSubsystem::Get(WorldContextObject->GetWorld()))
	{
		if (ConfigSubsystem->AbilityConfigDataTable)
		{
			if (const auto AbilityConfig = ConfigSubsystem->AbilityConfigDataTable->FindRow<FSimpleTaskDataTableLine>(
				AbilityName, "Row"))
			{
				AARPGCastAction* Action = CreateARPGAction<AARPGCastAction>(
					WorldContextObject, ActionOwnerCharacter, ActionExclusiveGroupID);
				Action->ActionTaskStructs = AbilityConfig->SpellTasks;
				Action->InitTaskObjects();
				return Action;
			}
		}
	}
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


UARPGSimpleTask* UARPGSimpleTask::CreateSimpleTask(AARPGCastAction* TaskOwnerAction, float TaskStartTime,
                                                   float TaskDuration,
                                                   TSubclassOf<AARPGSpecialEffectCreature> TaskCreateSpecialEffectCreatureClass,
                                                   FTransform RelativeTransform)
{
	UARPGSimpleTask* Task = NewObject<UARPGSimpleTask>();
	Task->OwnerAction = TaskOwnerAction;
	Task->StartTime = TaskStartTime;
	Task->TimeRemaining = TaskDuration;
	Task->SpecialEffectCreatureClass = TaskCreateSpecialEffectCreatureClass;
	Task->Transform = RelativeTransform;
	return Task;
}

void UARPGSimpleTask::OnTaskExecuted()
{
	Super::OnTaskExecuted();

	SpecialEffectCreature = UARPGGameInstanceSubsystem::SpawnActor<AARPGSpecialEffectCreature>(
        SpecialEffectCreatureClass, Transform, OwnerAction->OwnerCharacter);
}

void UARPGSimpleTask::OnTaskFinished()
{
	Super::OnTaskFinished();
}


UTask* UTask::CreateTask(AARPGCastAction* TaskOwnerAction, float TaskStartTime, float TaskDuration,
                         FTaskDelegate TaskOnTaskExecuted, FTaskDelegate TaskOnTaskFinished)
{
	if (UTask* Task = NewObject<UTask>(TaskOwnerAction))
	{
		Task->StartTime = TaskStartTime;
		Task->TimeRemaining = TaskDuration;
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
