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
    Super::ActivateAction(Target);

    if (IsMeleeAttacking)
    {
        FinishAction();
        return;
    }

    OwningCharacter->PlayAnimMontage(MeleeAttackMontages[MeleeAttackIndex]);
}

void AARPGMeleeAttackAction::OnMontageBegin(UAnimMontage* Montage)
{
    IsMeleeAttacking = true;
}

void AARPGMeleeAttackAction::OnMontageNotify(FName NotifyName,
                                             const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    IsMeleeAttacking = false;
    MeleeAttackIndex = (MeleeAttackIndex + 1) % MeleeAttackMontages.Num();
    FinishAction();
}

void AARPGMeleeAttackAction::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        MeleeAttackIndex = 0;
        IsMeleeAttacking = false;
    }
}

void AARPGMeleeAttackAction::Interrupt(AARPGCharacter* Causer)
{
    MeleeAttackIndex = 0;
    IsMeleeAttacking = false;

    Super::Interrupt(Causer);
}
