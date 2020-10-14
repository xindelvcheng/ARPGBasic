#include "ARPGAction.h"
#include "ARPGCharacter.h"

void AARPGAction::FinishAction()
{
    OnActionFinished.ExecuteIfBound();
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

void AARPGSimpleMontageAction::ActivateAction(AARPGCharacter* Target)
{
    Super::ActivateAction(Target);
    OwningCharacter->PlayAnimMontage(ActionMontage);
}

void AARPGSimpleMontageAction::Interrupt(AARPGCharacter* Causer)
{
    AttachedCharacterAnimInstance->StopAllMontages(0.5);
    Super::Interrupt(Causer);
}


void AARPGSimpleMontageAction::InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter)
{
    Super::InitWithOwningCharacter(NewOwningCharacter);

    if (OwningCharacter && OwningCharacter->GetMesh())
    {
        AttachedCharacterAnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
        if (AttachedCharacterAnimInstance)
        {
            AttachedCharacterAnimInstance->OnMontageStarted.AddDynamic(
                this, &AARPGSimpleMontageAction::OnMontageBegin);
            AttachedCharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(
                this, &AARPGSimpleMontageAction::OnMontageNotify);
            AttachedCharacterAnimInstance->OnMontageEnded.AddDynamic(
                this, &AARPGSimpleMontageAction::OnMontageStop);
        }
    }
}


void AARPGMeleeAttackAction::ActivateAction(AARPGCharacter* Target)
{
    ActionMontage = MeleeAttackMontages[MeleeAttackIndex];
    Super::ActivateAction(Target);
}

void AARPGMeleeAttackAction::OnMontageBegin(UAnimMontage* Montage)
{
    Super::OnMontageBegin(Montage);
}

void AARPGMeleeAttackAction::OnMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    Super::OnMontageNotify(NotifyName, BranchingPointPayload);
    
    MeleeAttackIndex = (MeleeAttackIndex + 1) % MeleeAttackMontages.Num();
    FinishAction();
}

void AARPGMeleeAttackAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        MeleeAttackIndex = 0;
        Super::OnMontageStop(Montage, bInterrupted);
    }
}

void AARPGMeleeAttackAction::Interrupt(AARPGCharacter* Causer)
{
    MeleeAttackIndex = 0;

    Super::Interrupt(Causer);
}
