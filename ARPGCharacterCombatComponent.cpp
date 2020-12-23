// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"
#include "ARPGCastAction.h"
#include "ARPGGameInstanceSubsystem.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UARPGCharacterCombatComponent::UARPGCharacterCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

template <typename T>
void UARPGCharacterCombatComponent::SpawnActionActors(const TArray<TSubclassOf<T>>& ActionClasses,
                                                      TArray<T*>& ActionActors)
{
	for (const auto ActionClass : ActionClasses)
	{
		if (ActionClass)
		{
			ActionActors.Emplace(AARPGAction::CreateARPGAction<T>(ActionClass, GetOwnerCharacter(),
                                                              FActionFinishDelegate::CreateUObject(
                                                                  this,
                                                                  &UARPGCharacterCombatComponent::BindToOnActionFinished)));
		}else
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(FString::Printf(TEXT("%s的Action存在NULL"),*GetOwner()->GetName()));
		}
	}
}


// Called when the game starts
void UARPGCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	ReInitCharacterActions();
}


// Called every frame
void UARPGCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// ...
}

void UARPGCharacterCombatComponent::BindToOnActionFinished(AARPGAction* Action)
{
	if (ExclusiveGroupActionsMap.FindRef(Action->GetActionExclusiveGroupID()) == Action)
	{
		ExclusiveGroupActionsMap.Remove(Action->GetActionExclusiveGroupID());
	}
}


bool UARPGCharacterCombatComponent::TryToMeleeAttack()
{
	if (IsRigid || !CurrentMeleeAttackCollection || ExclusiveGroupActionsMap.Contains(
		CurrentMeleeAttackCollection->GetActionExclusiveGroupID()))
	{
		return false;
	}
	if (CurrentMeleeAttackCollection->TryToActivateAction(GetOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(CurrentMeleeAttackCollection->GetActionExclusiveGroupID(),
		                             CurrentMeleeAttackCollection);
		CurrentActiveAction = CurrentMeleeAttackCollection;
		return true;
	}
	return false;
}

bool UARPGCharacterCombatComponent::TryToRemoteAttack(int RemoteAttackIndex = 0)
{
	if (IsRigid || !RemoteAttackActions.IsValidIndex(RemoteAttackIndex) || ExclusiveGroupActionsMap.Contains(
		RemoteAttackActions[RemoteAttackIndex]->GetActionExclusiveGroupID()))
	{
		return false;
	}

	AARPGAction* RemoteAttackAction = RemoteAttackActions[RemoteAttackIndex];
	if (RemoteAttackAction->TryToActivateAction(GetOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(RemoteAttackAction->GetActionExclusiveGroupID(), RemoteAttackAction);
		CurrentActiveAction = RemoteAttackAction;
		return true;
	}
	return false;
}

bool UARPGCharacterCombatComponent::TryToUseAbility(int AbilityIndex = 0)
{
	if (IsRigid || !AbilityActions.IsValidIndex(AbilityIndex) || ExclusiveGroupActionsMap.Contains(
		AbilityActions[AbilityIndex]->GetActionExclusiveGroupID()))
	{
		return false;
	}

	AARPGCastAction* Ability = AbilityActions[AbilityIndex];
	Ability->SetActorTransform(
		UARPGGameInstanceSubsystem::GetActorNearPositionTransform(GetOwnerCharacter(),
		                                                          {Ability->GetMaxDistance(), 0, 0}, FRotator{}));
	if (Ability->TryToActivateAction(GetOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(Ability->GetActionExclusiveGroupID(), Ability);
		CurrentActiveAction = Ability;
		return true;
	}
	return false;
}

bool UARPGCharacterCombatComponent::CauseRigid(float Duration, AARPGCharacter* Causer = nullptr)
{
	IsRigid = true;
	//打断当前的攻击和技能
	if (CurrentActiveAction)
	{
		CurrentActiveAction->InterruptAction(Causer);
	}
	FTimerManager& WorldTimeManager = GetWorld()->GetTimerManager();

	const float TimerRemaining = Duration > WorldTimeManager.GetTimerRemaining(RigidTimerHandle)
		                             ? Duration
		                             : WorldTimeManager.GetTimerRemaining(RigidTimerHandle);
	WorldTimeManager.SetTimer(RigidTimerHandle,
	                          FTimerDelegate::CreateLambda([&]()
	                          {
		                          GetOwnerCharacter()->GetCharacterMovement()->Activate();
		                          IsRigid = false;
		                          OnResumeFromRigid.Broadcast(WorldTimeManager.GetTimerElapsed(RigidTimerHandle));
	                          }),
	                          TimerRemaining
	                          , false);
	OnRigid.Broadcast(TimerRemaining);

	ActivateBuff(0, 0.3, Causer);
	GetOwnerCharacter()->GetCharacterMovement()->Deactivate();

	return true;
}

bool UARPGCharacterCombatComponent::ActivateBuff(int BuffIndex, float Duration, AARPGCharacter* Instigator)
{
	if (IsRigid || !BuffActions.IsValidIndex(BuffIndex) || ExclusiveGroupActionsMap.Contains(
		BuffActions[BuffIndex]->GetActionExclusiveGroupID()))
	{
		return false;
	}

	AARPGBuff* Buff = BuffActions[BuffIndex];
	Buff->SetDuration(Duration);
	if (Buff->TryToActivateAction(GetOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(Buff->GetActionExclusiveGroupID(), Buff);
		return true;
	}
	return false;
}

void UARPGCharacterCombatComponent::ReInitCharacterActions(
	UCharacterConfigPrimaryDataAsset* CharacterConfigPrimaryDataAsset)
{
	MeleeAttackCollectionActions.Empty();
	RemoteAttackActions.Empty();
	AbilityActions.Empty();
	BuffActions.Empty();

	if (CharacterConfigPrimaryDataAsset)
	{
		MeleeAttackCollectionClasses = CharacterConfigPrimaryDataAsset->MeleeAttackCollectionClasses;
		RemoteAttackClasses = CharacterConfigPrimaryDataAsset->RemoteAttackClasses;
		AbilityClasses = CharacterConfigPrimaryDataAsset->AbilityClasses;
		BuffClasses = CharacterConfigPrimaryDataAsset->BuffClasses;
		SpellNames = CharacterConfigPrimaryDataAsset->SpellNames;
	}

	MeleeAttackCollectionClasses.RemoveAll([](auto e){return !e;});
	RemoteAttackClasses.RemoveAll([](auto e){return !e;});
	AbilityClasses.RemoveAll([](auto e){return !e;});
	BuffClasses.RemoveAll([](auto e){return !e;});

	SpawnActionActors(MeleeAttackCollectionClasses, MeleeAttackCollectionActions);
	SpawnActionActors(RemoteAttackClasses, RemoteAttackActions);
	SpawnActionActors(BuffClasses, BuffActions);

	
	for (auto SpellName : SpellNames)
	{
		if (AARPGMeleeAttackAction* MeleeAction = Cast<AARPGMeleeAttackAction>(CurrentMeleeAttackCollection))
		{
			AARPGCastAction* Action = AARPGCastAction::Create(GetOwnerCharacter(), SpellName,
                                                              FActionFinishDelegate::CreateUObject(
                                                                  this,
                                                                  &UARPGCharacterCombatComponent::BindToOnActionFinished));
			AbilityActions.Emplace(Action);
		}
	}

	CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
}
