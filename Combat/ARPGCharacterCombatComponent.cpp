// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"
#include "ARPGAimComponent.h"
#include "ARPGCastAction.h"
#include "ARPGCoreSubsystem.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "ARPGLockTargetComponent.h"
#include "ARPGStaticFunctions.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GamePlay/TranscendentalCombatComponent.h"


// Sets default values for this component's properties
UARPGCharacterCombatComponent::UARPGCharacterCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UARPGCharacterCombatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	/*GetGameInstance不为nullptr确保在游戏运行中（而不是编辑器中）*/
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		ReInitCharacterActions();
	}
}


void UARPGCharacterCombatComponent::ReInitCharacterActions()
{
	MeleeAttackCollectionActions.Empty();
	AbilityActions.Empty();
	BuffActions.Empty();

	if (bUseCharacterDataAssetInit)
	{
		if (!GetDirectlyOwnerCharacter())
		{
			return;
		}

		if (!GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset())
		{
			UARPGStaticFunctions::PrintLogToScreen(
				FString::Printf(
					TEXT("%s设置bUseCharacterDataAssetInit=true却没有配置CharacterConfigDataAsset"),
					*GetDirectlyOwnerCharacter()->GetName()));
		}
		MeleeAttacks = GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset()->MeleeAttacks;
		AbilityClasses = GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset()->AbilityClasses;
		BuffClasses = GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset()->BuffClasses;
		SpellNames = GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset()->SpellNames;
	}

	AbilityClasses.RemoveAll([](auto e) { return !e; });
	BuffClasses.RemoveAll([](auto e) { return !e; });

	for (const auto MeleeAttackActionDescriptionStruct : MeleeAttacks)
	{
		if (AARPGMeleeAttackAction* Action = AARPGAction::CreateARPGAction<AARPGMeleeAttackAction>(
			AARPGMeleeAttackAction::StaticClass(), GetDirectlyOwnerCharacter(),
			FActionFinishDelegate::CreateUObject(
				this,
				&UARPGCharacterCombatComponent::BindToActionFinished)))
		{
			Action->MeleeAttackMontages = MeleeAttackActionDescriptionStruct.MeleeAttackMontages;
			MeleeAttackCollectionActions.Emplace(Action);
		}
		else
		{
			UARPGStaticFunctions::PrintLogToScreen(
				FString::Printf(
					TEXT("生成%s的AARPGMeleeAttackAction发生错误"), GetOwner() ? *GetOwner()->GetName() : *this->GetName()));
		}
	}
	if (MeleeAttackCollectionActions.Num() > 0)
	{
		CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
	}
	else if (GetDirectlyOwnerCharacter()->GetCharacterConfigDataAsset())
	{
		UARPGStaticFunctions::PrintLogToScreen(
			FString::Printf(TEXT("%s的DataAsset未配置MeleeAttacks"), *GetOwner()->GetName()));
	}


	for (auto SpellName : SpellNames)
	{
		if (AARPGMeleeAttackAction* MeleeAction = Cast<AARPGMeleeAttackAction>(CurrentMeleeAttackCollection))
		{
			AARPGCastAction* Action = AARPGCastAction::Create(GetDirectlyOwnerCharacter(), SpellName,
			                                                  FActionFinishDelegate::CreateUObject(
				                                                  this,
				                                                  &UARPGCharacterCombatComponent::BindToActionFinished));
			AbilityActions.Emplace(Action);
		}
	}

	for (const auto ActionClass : AbilityClasses)
	{
		if (ActionClass)
		{
			AARPGCastAction* ARPGAction = AARPGAction::CreateARPGAction<AARPGCastAction>(
				ActionClass, GetDirectlyOwnerCharacter(),
				FActionFinishDelegate::CreateUObject(
					this,
					&UARPGCharacterCombatComponent::BindToActionFinished));
			AbilityActions.Emplace(ARPGAction);
			AbilityActionsMap.Emplace(ARPGAction->GetItemName(), ARPGAction);
		}
		else
		{
			UARPGStaticFunctions::PrintLogToScreen(
				FString::Printf(TEXT("%s的Action存在NULL"), *GetOwner()->GetName()));
		}
	}


	for (const auto ActionClass : BuffClasses)
	{
		if (ActionClass)
		{
			BuffActions.Emplace(AARPGAction::CreateARPGAction<AARPGBuff>(ActionClass, GetDirectlyOwnerCharacter(),
			                                                             FActionFinishDelegate::CreateUObject(
				                                                             this,
				                                                             &UARPGCharacterCombatComponent::BindToActionFinished)));
		}
		else
		{
			UARPGStaticFunctions::PrintLogToScreen(
				FString::Printf(TEXT("%s的Action存在NULL"), *GetOwner()->GetName()));
		}
	}
}


// Called when the game starts
void UARPGCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UARPGCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// ...
}

void UARPGCharacterCombatComponent::BindToActionFinished(AARPGAction* Action)
{
	if (ExclusiveGroupActionsMap.FindRef(Action->GetActionExclusiveGroupID()) == Action)
	{
		ExclusiveGroupActionsMap.Remove(Action->GetActionExclusiveGroupID());
	}

	if (Cast<AARPGMeleeAttackAction>(Action))
	{
		MeleeAttackEndDelegate.Broadcast();
	}
}


bool UARPGCharacterCombatComponent::TryToMeleeAttack()
{
	if (IsRigid || !CurrentMeleeAttackCollection || ExclusiveGroupActionsMap.Contains(
		CurrentMeleeAttackCollection->GetActionExclusiveGroupID()))
	{
		return false;
	}
	if (CurrentMeleeAttackCollection->TryToActivateAction(GetDirectlyOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(CurrentMeleeAttackCollection->GetActionExclusiveGroupID(),
		                             CurrentMeleeAttackCollection);
		CurrentActiveAction = CurrentMeleeAttackCollection;
		MeleeAttackStartDelegate.Broadcast();
		return true;
	}
	return false;
}

bool UARPGCharacterCombatComponent::TryToCastSpell(AARPGCastAction* Spell)
{
	
	if (IsRigid || !Spell || ExclusiveGroupActionsMap.Contains(Spell->GetActionExclusiveGroupID()))
	{
		return false;
	}

	UARPGAimComponent* AimComponent = GetOwnerCharacter()->GetAimComponent();
	if (AimComponent && AimComponent->IsAimTargetResultIsValid())
	{
		Spell->SetActorTransform(AimComponent->GetAimTargetActor()->GetActorTransform());
	}
	else
	{
		UARPGLockTargetComponent* LockTargetComponent = GetDirectlyOwnerCharacter()->GetCharacterLockTargetComponent();

		if (AARPGCharacter* LockingTarget = LockTargetComponent->GetLockingTarget())
		{
			Spell->SetActorTransform(LockingTarget->GetActorTransform());
		}
		else if (AARPGCharacter* LockTarget = LockTargetComponent->DetectLockTarget())
		{
			Spell->SetActorTransform(LockTarget->GetActorTransform());
		}
		else
		{
			Spell->SetActorTransform(
				UARPGStaticFunctions::ConvActorLocalTransformToWorldTransform(GetDirectlyOwnerCharacter(),
				                                                    {
					                                                    Spell->GetMaxDistance(), 0, 0
				                                                    }));
		}
	}


	if (Spell->TryToActivateAction(GetDirectlyOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(Spell->GetActionExclusiveGroupID(), Spell);
		CurrentActiveAction = Spell;
		return true;
	}
	return false;
}

bool UARPGCharacterCombatComponent::CauseRigid(float Duration, AARPGCharacter* Causer = nullptr)
{
	IsRigid = true;
	//打断当前的攻击和技能，因为取消了技能后摇，可能造成Character鬼畜（如果不间断地使用技能）
	if (CurrentActiveAction)
	{
		CurrentActiveAction->FinishAction();
	}
	FTimerManager& WorldTimeManager = GetWorld()->GetTimerManager();

	const float TimerRemaining = Duration > WorldTimeManager.GetTimerRemaining(RigidTimerHandle)
		                             ? Duration
		                             : WorldTimeManager.GetTimerRemaining(RigidTimerHandle);
	WorldTimeManager.SetTimer(RigidTimerHandle,
	                          FTimerDelegate::CreateLambda([&]()
	                          {
		                          if (GetDirectlyOwnerCharacter())
		                          {
			                          GetDirectlyOwnerCharacter()->GetCharacterMovement()->Activate();
			                          IsRigid = false;
			                          OnResumeFromRigid.Broadcast(WorldTimeManager.GetTimerElapsed(RigidTimerHandle));
		                          }
	                          }),
	                          TimerRemaining
	                          , false);
	OnRigid.Broadcast(TimerRemaining);

	ActivateBuff(0, 0.3, Causer);
	GetDirectlyOwnerCharacter()->GetCharacterMovement()->Deactivate();

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
	if (Buff->TryToActivateAction(GetDirectlyOwnerCharacter()))
	{
		ExclusiveGroupActionsMap.Add(Buff->GetActionExclusiveGroupID(), Buff);
		return true;
	}
	return false;
}


UActivateActionBlueprintNode* UActivateActionBlueprintNode::ActivateAction(AARPGCharacter* Instigator, FName ActionName, bool& bActivateSuccess)
{
	UActivateActionBlueprintNode* ActionBlueprintNode = NewObject<UActivateActionBlueprintNode>();
	bActivateSuccess = ActionBlueprintNode->ActivateActionInternal(Instigator, ActionName);
	return ActionBlueprintNode;
}

bool UActivateActionBlueprintNode::ActivateActionInternal(AARPGCharacter* Instigator, FName ActionName)
{
	if (!Instigator)
	{
		return false;
	}
	if (const auto CharacterCombatComponent = Instigator->GetCharacterCombatComponent())
	{
		AARPGCastAction* Action = nullptr;
		for (AARPGCastAction* e : CharacterCombatComponent->GetAbilityActions())
		{
			if (e && e->GetItemName() == ActionName)
			{
				Action = e;
				break;
			}
		}
		Action->OnActionFinishedEvent().AddLambda([this](AARPGAction*)
		{
			ActionFinished.Broadcast();
		});
		return CharacterCombatComponent->TryToCastSpell(Action);
	}
	return false;
}
