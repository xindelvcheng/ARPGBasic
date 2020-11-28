﻿#include "ARPGAction.h"
#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"


bool AARPGAction::CheckConditionAndPayCost()
{
	return BPFunc_CheckConditionAndPayCost();
}

bool AARPGAction::BPFunc_CheckConditionAndPayCost_Implementation()
{
	const int SPCost = static_cast<int>(static_cast<float>(OwningCharacter->GetMaxSP()) / 4);

	if (OwningCharacter->GetCurrentSP() > SPCost)
	{
		OwningCharacter->UpdateCurrentSP(-SPCost);
		return true;
	}
	return false;
}

void AARPGAction::FinishAction()
{
	OnActionFinished.ExecuteIfBound(this);
}

void AARPGAction::InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter)
{
	OwningCharacter = NewOwningCharacter;
}

bool AARPGAction::TryToActivateAction(AARPGCharacter* User, AARPGCharacter* Target)
{
	if (OwningCharacter != User)
	{
		InitWithOwningCharacter(User);
	}
	verifyf(OwningCharacter, TEXT("未显式调用InitWithOwningCharacter，也没有在激活Actor时指定OwningCharacter"));

	if (CheckConditionAndPayCost())
	{
		OnActionActivate();
		return true;
	}
	return false;
}

void AARPGAction::OnActionActivate()
{
	BPFunc_Active();
}


void AARPGAction::Interrupt(AARPGCharacter* Causer)
{
	FinishAction();
}

void AARPGMontageAction::OnActionActivate()
{
	Super::OnActionActivate();
	OwningCharacter->PlayAnimMontage(ActionMontage, PlayRate, StartSectionName);
}

void AARPGMontageAction::BindToMontageBegin(UAnimMontage* Montage)
{
	if (Montage != ActionMontage)
	{
		return;
	}

	if (FAnimMontageInstance* MontageInstance = AttachedCharacterAnimInstance->GetActiveInstanceForMontage(
		ActionMontage))
	{
		MontageInstanceID = MontageInstance->GetInstanceID();
	}
	OnMontageBegin(Montage);
	BPFunc_OnMontageBegin(OwningCharacter);
}

void AARPGMontageAction::BindToMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (BranchingPointPayload.MontageInstanceID != MontageInstanceID)
	{
		return;
	}
	OnMontageNotify(NotifyName, BranchingPointPayload);
	BPFunc_OnMontageNotify(NotifyName, OwningCharacter);
}

void AARPGMontageAction::BindToMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActionMontage)
	{
		return;
	}
	OnMontageStop(Montage, bInterrupted);
	BPFunc_OnMontageStop(OwningCharacter);
}

void AARPGMontageAction::Interrupt(AARPGCharacter* Causer)
{
	AttachedCharacterAnimInstance->Montage_Stop(0.2, ActionMontage);
	Super::Interrupt(Causer);
	BPFunc_Interrupt(OwningCharacter);
}


void AARPGMontageAction::InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter)
{
	Super::InitWithOwningCharacter(NewOwningCharacter);

	if (OwningCharacter && OwningCharacter->GetMesh())
	{
		AttachedCharacterAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		if (AttachedCharacterAnimInstance)
		{
			AttachedCharacterAnimInstance->OnMontageStarted.AddDynamic(
				this, &AARPGMontageAction::BindToMontageBegin);
			AttachedCharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
				this, &AARPGMontageAction::BindToMontageNotify);
			AttachedCharacterAnimInstance->OnMontageEnded.AddDynamic(
				this, &AARPGMontageAction::BindToMontageStop);
		}
	}
}


void AARPGSingleMontageAction::OnActionActivate()
{
	ActionMontage = ActionMontageAsset;
	Super::OnActionActivate();
}

void AARPGSingleMontageAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	FinishAction();
}

void AARPGMeleeAttackAction::OnActionActivate()
{
	ActionMontage = MeleeAttackMontages[MeleeAttackIndex];
	Super::OnActionActivate();
}

void AARPGMeleeAttackAction::OnMontageBegin(UAnimMontage* Montage)
{
}

void AARPGMeleeAttackAction::OnMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName.ToString() == TEXT("AppendAttack") || NotifyName == NAME_None)
	{
		MeleeAttackIndex = (MeleeAttackIndex + 1) % MeleeAttackMontages.Num();
		FinishAction();
	}
}

void AARPGMeleeAttackAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		MeleeAttackIndex = 0;
	}
}

void AARPGMeleeAttackAction::Interrupt(AARPGCharacter* Causer)
{
	MeleeAttackIndex = 0;

	Super::Interrupt(Causer);
}

void AARPGMultiMontageAction::OnActionActivate()
{
	ActionMontage = ActionMontages[ActionIndex];
	Super::OnActionActivate();
}

void AARPGMultiMontageAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetWorldTimerManager().TimerExists(ResetTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ResetTimerHandle);
	}
	ActionIndex = (ActionIndex + 1) % ActionMontages.Num();
	GetWorldTimerManager().SetTimer(ResetTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		ActionIndex = 0;
	}), ResetDelay, false);
}

void AARPGBuff::OnActionActivate()
{
	Super::OnActionActivate();

	GetWorldTimerManager().SetTimer(BuffTimerHandle,
	                                FTimerDelegate::CreateLambda([&]() { FinishAction(); }),
	                                BuffDuration > GetWorldTimerManager().GetTimerRemaining(BuffTimerHandle)
		                                ? BuffDuration
		                                : GetWorldTimerManager().GetTimerRemaining(BuffTimerHandle), false);
}
