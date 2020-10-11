// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
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


// Sets default values for this component's properties
UARPGCharacterCombatComponent::UARPGCharacterCombatComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

void UARPGCharacterCombatComponent::SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& ActionClasses,
    TArray<AARPGAction*>& ActionActors)
{
    for (auto ActionClass : ActionClasses)
    {
        AARPGAction* Ability = Cast<AARPGAction>(GetWorld()->SpawnActor(ActionClass));
        check(Ability);
        Ability->InitWithOwningCharacter(AttachedCharacter);
        ActionActors.Add(Ability);
        Ability->OnActionFinished.BindUObject(this, &UARPGCharacterCombatComponent::BindToOnActionFinished);
    }
}


// Called when the game starts
void UARPGCharacterCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MeleeAttackClasses.Num()>0);

    // ...
    AttachedCharacter = Cast<AARPGCharacter>(GetOwner());
    check(AttachedCharacter);

    RigidTimerDelegate.BindLambda([&]()
    {
        IsRigid = false;
        OnResumeFromRigid.Broadcast(GetWorld()->GetTimerManager().GetTimerElapsed(RigidTimerHandle));
    });

    SpawnActionActors(MeleeAttackClasses,MeleeAttackActions);
    SpawnActionActors(RemoteAttackClasses,RemoteAttackActions);
    SpawnActionActors(AbilityClasses,AbilityActions);
    SpawnActionActors(BuffClasses,BuffActions);
}

void UARPGCharacterCombatComponent::BindToOnActionFinished()
{
    IsActing = false;
}


// Called every frame
void UARPGCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}


void UARPGCharacterCombatComponent::TryToMeleeAttack(int NormalAttackCollectionIndex)
{
    if (IsActing || IsRigid)
    {
        return;
    }
    

    if (MeleeAttackActions.IsValidIndex(NormalAttackCollectionIndex))
    {
        AARPGAction* MeleeAttackAction = MeleeAttackActions[NormalAttackCollectionIndex];
        MeleeAttackAction->ActivateAction(AttachedCharacter);
        IsActing = true;
        CurrentActiveAction = MeleeAttackAction;
    }
}

void UARPGCharacterCombatComponent::TryToRemoteAttack(int RemoteAttackIndex = 0)
{
    if (IsActing || IsRigid)
    {
        return;
    }

    if (RemoteAttackActions.IsValidIndex(RemoteAttackIndex))
    {
        AARPGAction* RemoteAttackAction = RemoteAttackActions[RemoteAttackIndex];
        RemoteAttackAction->ActivateAction(AttachedCharacter);
        IsActing = true;
        CurrentActiveAction = RemoteAttackAction;
    }
}

void UARPGCharacterCombatComponent::TryToUseAbility(int AbilityIndex = 0)
{
    if (IsActing || IsRigid)
    {
        return;
    }

    if (AbilityActions.IsValidIndex(AbilityIndex))
    {
        AARPGAction* Ability = AbilityActions[AbilityIndex];
        Ability->ActivateAction(AttachedCharacter);
        IsActing = true;
        CurrentActiveAction = Ability;
    }
}

void UARPGCharacterCombatComponent::CauseRigid(float Duration, AARPGCharacter* Causer = nullptr)
{
    //打断当前的攻击和技能
    IsActing = false;
    if (CurrentActiveAction)
    {
        CurrentActiveAction->Interrupt(Causer);
    }

    if (!GetWorld()->GetTimerManager().TimerExists(RigidTimerHandle))
    {
        GetWorld()->GetTimerManager().SetTimer(RigidTimerHandle, RigidTimerDelegate, Duration, false);
        OnRigid.Broadcast(Duration);
    }
    else
    {
        float TimerRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(RigidTimerHandle);
        TimerRemaining = Duration > TimerRemaining ? Duration : TimerRemaining;
        GetWorld()->GetTimerManager().ClearTimer(RigidTimerHandle);
        GetWorld()->GetTimerManager().SetTimer(RigidTimerHandle, RigidTimerDelegate, TimerRemaining
                                               , false);
        OnRigid.Broadcast(TimerRemaining);
    }
    IsRigid = true;
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
