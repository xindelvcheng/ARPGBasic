#include "ARPGAction.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"


bool AARPGAction::CheckActionActivateConditionAndPayCost()
{
	return BPFunc_CheckActionActivateConditionAndPayCost();
}

bool AARPGAction::BPFunc_CheckActionActivateConditionAndPayCost_Implementation()
{
	const int SPCost = static_cast<int>(static_cast<float>(GetOwnerCharacter()->GetMaxSP()) / 4);

	if (GetOwnerCharacter() && GetOwnerCharacter()->GetCurrentSP() > SPCost)
	{
		GetOwnerCharacter()->UpdateCurrentSP(-SPCost);
		return true;
	}
	return false;
}

void AARPGAction::BeginPlay()
{
	Super::BeginPlay();
}

AARPGAction::AARPGAction()
{
}

void AARPGAction::FinishAction()
{
	SetActorTickEnabled(false);

	OnActionFinished(this);
	ActionFinishedEvent.Broadcast(this);
}

void AARPGAction::InterruptAction(AARPGCharacter* Causer)
{
	FinishAction();

	OnActionInterrupted(Causer);
	BPFunc_OnActionInterrupted(Causer);
}

void AARPGAction::InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter)
{
	SetOwnerCharacter(NewOwningCharacter);
}

bool AARPGAction::TryToActivateAction(AARPGCharacter* User, AARPGCharacter* Target)
{
	verifyf(GetOwnerCharacter(), TEXT("未显式调用InitWithOwningCharacter，也没有在激活Actor时指定OwningCharacter"));

	if (CheckActionActivateConditionAndPayCost())
	{
		OnActionActivate();
		return true;
	}
	return false;
}

void AARPGAction::OnActionActivate()
{
	BPFunc_OnActionActivate();
}


template <typename T>
T* AARPGAction::CreateARPGAction(TSubclassOf<AARPGAction> ActionClass,
                                 AARPGCharacter* ActionOwnerCharacter, FActionFinishDelegate ActionFinishedDelegate,
                                 int ActionExclusiveGroupID)
{
	return CreateARPGAction<T>(ActionClass, ActionOwnerCharacter, FTransform{}, ActionFinishedDelegate,
	                           ActionExclusiveGroupID);
}

template <typename T>
T* AARPGAction::CreateARPGAction(TSubclassOf<AARPGAction> ActionClass, AARPGCharacter* ActionOwnerCharacter,
                                 FTransform Transform,
                                 FActionFinishDelegate ActionFinishedDelegate, int ActionExclusiveGroupID)
{
	if (T* Action = ActionOwnerCharacter->GetWorld()->SpawnActorDeferred<T>(
		ActionClass, Transform, ActionOwnerCharacter, ActionOwnerCharacter,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		Action->ExclusiveGroupID = ActionExclusiveGroupID;
		Action->ActionFinishedEvent.Add(ActionFinishedDelegate);
		Action->FinishSpawning(Transform);
		return Action;
	};
	UARPGGameInstanceSubsystem::PrintLogToScreen(
		FString::Printf(TEXT("%s生成Action出现错误"), *ActionOwnerCharacter->GetName()));
	return nullptr;
}

void AARPGMontageAction::BindDelegateToOwnerCharacterAnimInstance()
{
	if (GetOwnerCharacter() && GetOwnerCharacter()->GetMesh() && !AttachedCharacterAnimInstance)
	{
		AttachedCharacterAnimInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance();
		if (AttachedCharacterAnimInstance)
		{
			if (!AttachedCharacterAnimInstance->OnMontageStarted.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageBegin))
			{
				AttachedCharacterAnimInstance->OnMontageStarted.AddDynamic(
					this, &AARPGMontageAction::BindToMontageBegin);
			}
			if (!AttachedCharacterAnimInstance->OnPlayMontageNotifyBegin.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageNotify))
			{
				AttachedCharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
					this, &AARPGMontageAction::BindToMontageNotify);
			}
			if (!AttachedCharacterAnimInstance->OnMontageEnded.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageStop))
			{
				AttachedCharacterAnimInstance->OnMontageEnded.AddDynamic(
					this, &AARPGMontageAction::BindToMontageStop);
			}
		}
	}
}

void AARPGMontageAction::OnActionActivate()
{
	Super::OnActionActivate();
	GetOwnerCharacter()->PlayAnimMontage(ActionMontage, PlayRate, StartSectionName);
}

void AARPGMontageAction::SetOwnerCharacter(AARPGCharacter* NewOwner)
{
	Super::SetOwnerCharacter(NewOwner);

	BindDelegateToOwnerCharacterAnimInstance();
}

void AARPGMontageAction::BeginPlay()
{
	Super::BeginPlay();

	BindDelegateToOwnerCharacterAnimInstance();
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
	BPFunc_OnMontageBegin(GetOwnerCharacter());
}

void AARPGMontageAction::BindToMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (BranchingPointPayload.MontageInstanceID != MontageInstanceID)
	{
		return;
	}
	OnMontageNotify(NotifyName, BranchingPointPayload);
	BPFunc_OnMontageNotify(NotifyName, GetOwnerCharacter());
}

void AARPGMontageAction::BindToMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ActionMontage)
	{
		return;
	}
	OnMontageStop(Montage, bInterrupted);
	BPFunc_OnMontageStop(GetOwnerCharacter());
}

void AARPGMontageAction::OnActionInterrupted(AARPGCharacter* Causer)
{
	AttachedCharacterAnimInstance->Montage_Stop(0.2, ActionMontage);
	Super::OnActionInterrupted(Causer);
}


void AARPGSingleMontageAction::OnActionActivate()
{
	ActionMontage = ActionMontageAsset;
	Super::OnActionActivate();
}

void AARPGSingleMontageAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageStop(Montage, bInterrupted);

	FinishAction();
}

void AARPGMeleeAttackAction::OnActionActivate()
{
	ActionMontage = MeleeAttackMontages[MeleeAttackIndex];
	Super::OnActionActivate();
}

void AARPGMeleeAttackAction::OnMontageBegin(UAnimMontage* Montage)
{
	Super::OnMontageBegin(Montage);
}

void AARPGMeleeAttackAction::OnMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::OnMontageNotify(NotifyName, BranchingPointPayload);

	if (NotifyName.ToString() == TEXT("AppendAttack") || NotifyName == NAME_None)
	{
		MeleeAttackIndex = (MeleeAttackIndex + 1) % MeleeAttackMontages.Num();
		UARPGGameInstanceSubsystem::PrintLogToScreen(MeleeAttackIndex);
		FinishAction();
	}
}

void AARPGMeleeAttackAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageStop(Montage, bInterrupted);

	if (!bInterrupted)
	{
		MeleeAttackIndex = 0;
	}
}

void AARPGMeleeAttackAction::OnActionInterrupted(AARPGCharacter* Causer)
{
	MeleeAttackIndex = 0;

	Super::OnActionInterrupted(Causer);
}

void AARPGMultiMontageAction::OnActionActivate()
{
	ActionMontage = ActionMontages[ActionIndex];
	Super::OnActionActivate();
}

void AARPGMultiMontageAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageStop(Montage, bInterrupted);

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
