#include "ARPGAction.h"
#include "ARPGCharacter.h"


bool AARPGAction::CheckConditionAndPayCost_Implementation()
{
    int SPCost = static_cast<int>(static_cast<float>(OwningCharacter->GetMaxSP()) / 4);

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

void AARPGAction::ActivateAction(AARPGCharacter* Target)
{
    BPFunc_Active(Target);
}


void AARPGAction::Interrupt(AARPGCharacter* Causer)
{
    FinishAction();
}

void AARPGMontageAction::ActivateAction(AARPGCharacter* Target)
{
    Super::ActivateAction(Target);
    OwningCharacter->PlayAnimMontage(ActionMontage,PlayRate,StartSectionName);
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

void AARPGMontageAction::BindToMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (BranchingPointPayload.MontageInstanceID != MontageInstanceID)
    {
        return;
    }
    OnMontageNotify(NotifyName,BranchingPointPayload);
    BPFunc_OnMontageNotify(NotifyName, OwningCharacter);
}

void AARPGMontageAction::BindToMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ActionMontage)
    {
        return;
    }
    FinishAction();
    OnMontageStop(Montage,bInterrupted);
    BPFunc_OnMontageStop(OwningCharacter);
}

void AARPGMontageAction::Interrupt(AARPGCharacter* Causer)
{
    AttachedCharacterAnimInstance->StopAllMontages(0.5);
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


void AARPGSingleMontageAction::ActivateAction(AARPGCharacter* Target)
{
    ActionMontage = ActionMontageAsset;
    Super::ActivateAction(Target); 
}

void AARPGMeleeAttackAction::ActivateAction(AARPGCharacter* Target)
{
    ActionMontage = MeleeAttackMontages[MeleeAttackIndex];
    Super::ActivateAction(Target);
}

void AARPGMeleeAttackAction::OnMontageBegin(UAnimMontage* Montage)
{
    /*不知道该写些什么*/
}

void AARPGMeleeAttackAction::OnMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    MeleeAttackIndex = (MeleeAttackIndex + 1) % MeleeAttackMontages.Num();
    FinishAction();
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

void AARPGMultiMontageAction::ActivateAction(AARPGCharacter* Target)
{
    ActionMontage = ActionMontages[ActionIndex];
    Super::ActivateAction(Target);
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
