// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"
#include "ARPGAimComponent.h"
#include "ARPGCastAction.h"
#include "ARPGGameInstanceSubsystem.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "ARPGLockTargetComponent.h"

#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UARPGCharacterCombatComponent::UARPGCharacterCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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

#pragma optimize("",off)
bool UARPGCharacterCombatComponent::TryToUseAbility(int AbilityIndex = 0)
{
	if (IsRigid || !AbilityActions.IsValidIndex(AbilityIndex) || ExclusiveGroupActionsMap.Contains(
		AbilityActions[AbilityIndex]->GetActionExclusiveGroupID()))
	{
		return false;
	}

	AARPGCastAction* Ability = AbilityActions[AbilityIndex];

	UARPGAimComponent* AimComponent = GetOwnerCharacter()->GetAimComponent();
	if (AimComponent && AimComponent->IsAimTargetResultIsValid())
	{
		Ability->SetActorTransform(AimComponent->GetAimTargetActor()->GetActorTransform());
	}
	else
	{
		UARPGLockTargetComponent* LockTargetComponent = GetOwnerCharacter()->GetCharacterLockTargetComponent();
		if (AARPGCharacter* LockTarget = LockTargetComponent->DetectLockTarget())
		{
			Ability->SetActorTransform(LockTarget->GetActorTransform());
		}
		else
		{
			Ability->SetActorTransform(
				UARPGGameInstanceSubsystem::GetActorNearPositionTransform(GetOwnerCharacter(),
				                                                          {
					                                                          Ability->GetMaxDistance(), 0, 0
				                                                          }, FRotator{}));
		}
	}


	if (Ability->TryToActivateAction(GetOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(Ability->GetActionExclusiveGroupID(), Ability);
		CurrentActiveAction = Ability;
		return true;
	}
	return false;
}
#pragma optimize("",on)

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
		                          if (GetOwnerCharacter())
		                          {
			                          GetOwnerCharacter()->GetCharacterMovement()->Activate();
			                          IsRigid = false;
			                          OnResumeFromRigid.Broadcast(WorldTimeManager.GetTimerElapsed(RigidTimerHandle));
		                          }
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
		MeleeAttacks = CharacterConfigPrimaryDataAsset->MeleeAttacks;
		AbilityClasses = CharacterConfigPrimaryDataAsset->AbilityClasses;
		BuffClasses = CharacterConfigPrimaryDataAsset->BuffClasses;
		SpellNames = CharacterConfigPrimaryDataAsset->SpellNames;
	}

	AbilityClasses.RemoveAll([](auto e) { return !e; });
	BuffClasses.RemoveAll([](auto e) { return !e; });

	for (const auto MeleeAttackActionDescriptionStruct : MeleeAttacks)
	{
		if (AARPGMeleeAttackAction* Action = AARPGAction::CreateARPGAction<AARPGMeleeAttackAction>(
			AARPGMeleeAttackAction::StaticClass(), GetOwnerCharacter(),
			FActionFinishDelegate::CreateUObject(
				this,
				&UARPGCharacterCombatComponent::BindToOnActionFinished)))
		{
			Action->MeleeAttackMontages = MeleeAttackActionDescriptionStruct.MeleeAttackMontages;
			MeleeAttackCollectionActions.Emplace(Action);
		}
		else
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(
				FString::Printf(TEXT("生成%s的AARPGMeleeAttackAction发生错误"), *GetOwner()->GetName()));
		}
	}
	if (MeleeAttackCollectionActions.Num() > 0)
	{
		CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
	}
	else if (CharacterConfigPrimaryDataAsset)
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(
			FString::Printf(TEXT("%s的DataAsset未配置MeleeAttacks"), *GetOwner()->GetName()));
	}


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


	for (const auto ActionClass : BuffClasses)
	{
		if (ActionClass)
		{
			BuffActions.Emplace(AARPGAction::CreateARPGAction<AARPGBuff>(ActionClass, GetOwnerCharacter(),
			                                                             FActionFinishDelegate::CreateUObject(
				                                                             this,
				                                                             &UARPGCharacterCombatComponent::BindToOnActionFinished)));
		}
		else
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(
				FString::Printf(TEXT("%s的Action存在NULL"), *GetOwner()->GetName()));
		}
	}
}
