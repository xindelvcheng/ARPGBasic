#include "ARPGAction.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"
#include "GameFramework/GameModeBase.h"


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

	ActionState = EActionStateEnum::Stop;
}

AARPGAction::AARPGAction()
{
}

void AARPGAction::FinishAction()
{
	if (ActionState == EActionStateEnum::Run)
	{
		ActionState = EActionStateEnum::Stop;

		SetActorTickEnabled(false);

		OnActionFinished(this);
		ActionFinishedEvent.Broadcast(this);
	}
}

bool AARPGAction::TryToActivateAction(AARPGCharacter* NewUser, AARPGCharacter* NewTarget)
{
	verifyf(GetOwnerCharacter(), TEXT("未显式调用InitWithOwningCharacter，也没有在激活Actor时指定OwningCharacter"));

	if (CheckActionActivateConditionAndPayCost() && ActionState == EActionStateEnum::Stop)
	{
		ActionState = EActionStateEnum::Run;
		User = NewUser;
		Target = NewTarget;
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
	if (T* Action = GEngine->GetWorldFromContextObject(ActionOwnerCharacter, EGetWorldErrorMode::Assert)->
	                         SpawnActorDeferred<T>(
		                         ActionClass, Transform, ActionOwnerCharacter, ActionOwnerCharacter,
		                         ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		Action->ExclusiveGroupID = ActionExclusiveGroupID;
		Action->ActionFinishedEvent.Add(ActionFinishedDelegate);
		Action->FinishSpawning(Transform);
		return Action;
	};
	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("%s生成Action出现错误"), *ActionOwnerCharacter->GetName()));
	return nullptr;
}

void AARPGMontageAction::BindDelegateToOwnerCharacterAnimInstance()
{
	if (GetOwnerCharacter() && GetOwnerCharacter()->GetMesh() && !OwnerCharacterAnimInstance)
	{
		OwnerCharacterAnimInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance();
		if (OwnerCharacterAnimInstance)
		{
			if (!OwnerCharacterAnimInstance->OnMontageStarted.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageBegin))
			{
				OwnerCharacterAnimInstance->OnMontageStarted.AddDynamic(
					this, &AARPGMontageAction::BindToMontageBegin);
			}
			if (!OwnerCharacterAnimInstance->OnPlayMontageNotifyBegin.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageNotify))
			{
				OwnerCharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
					this, &AARPGMontageAction::BindToMontageNotify);
			}
			if (!OwnerCharacterAnimInstance->OnMontageEnded.IsAlreadyBound(
				this, &AARPGMontageAction::BindToMontageStop))
			{
				OwnerCharacterAnimInstance->OnMontageEnded.AddDynamic(
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

	if (FAnimMontageInstance* MontageInstance = OwnerCharacterAnimInstance->GetActiveInstanceForMontage(
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

void AARPGMontageAction::OnActionFinished(AARPGAction* Action)
{
	OwnerCharacterAnimInstance->Montage_Stop(0.2, ActionMontage);
	Super::OnActionFinished(Action);
}


void AARPGMeleeAttackAction::OnActionActivate()
{
	if (MeleeAttackMontages.IsValidIndex(MeleeAttackIndex))
	{
		SetActionMontage(MeleeAttackMontages[MeleeAttackIndex]);
	}
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

		/*广播此Action已结束（但并不实际调用FinishAction中止Montage）*/
		SetActionState(EActionStateEnum::Stop);
		OnActionFinishedEvent().Broadcast(this);
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

void AARPGMultiMontageAction::OnActionActivate()
{
	if (ActionMontages.IsValidIndex(MontageIndex))
	{
		SetActionMontage(ActionMontages[MontageIndex]);	
	}
	
	MontageIndex = (MontageIndex + 1) % ActionMontages.Num();
	Super::OnActionActivate();
}

void AARPGMultiMontageAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageStop(Montage, bInterrupted);

	GetWorldTimerManager().SetTimer(ResetTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		MontageIndex = 0;
	}), ResetDelay, false);
}

void AARPGMultiMontageAction::OnActionFinished(AARPGAction* Action)
{
	Super::OnActionFinished(Action);
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
