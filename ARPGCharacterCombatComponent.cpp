// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"
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
    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FTransform Transform;
    for (auto ActionClass : ActionClasses)
    {
        T* Action = Cast<T>(GetWorld()->SpawnActor(ActionClass, &Transform, ActorSpawnParameters));
        if (Action)
        {
            Action->InitWithOwningCharacter(AttachedCharacter);
            ActionActors.Add(Action);
            Action->OnActionFinished.BindUObject(this, &UARPGCharacterCombatComponent::BindToOnActionFinished);
        }
    }
}


// Called when the game starts
void UARPGCharacterCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MeleeAttackCollectionClasses.Num()>0);

    // ...
    AttachedCharacter = Cast<AARPGCharacter>(GetOwner());

    check(AttachedCharacter);


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
    if (CurrentMeleeAttackCollection->TryToActivateAction(AttachedCharacter))
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
    if (RemoteAttackAction->TryToActivateAction(AttachedCharacter))
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

    AARPGAction* Ability = AbilityActions[AbilityIndex];
    if (Ability->TryToActivateAction(AttachedCharacter))
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
        CurrentActiveAction->Interrupt(Causer);
    }
    FTimerManager& WorldTimeManager = GetWorld()->GetTimerManager();

    const float TimerRemaining = Duration > WorldTimeManager.GetTimerRemaining(RigidTimerHandle)
                                     ? Duration
                                     : WorldTimeManager.GetTimerRemaining(RigidTimerHandle);
    WorldTimeManager.SetTimer(RigidTimerHandle,
                              FTimerDelegate::CreateLambda([&]()
                              {
                                  AttachedCharacter->GetCharacterMovement()->Activate();
                                  IsRigid = false;
                                  OnResumeFromRigid.Broadcast(WorldTimeManager.GetTimerElapsed(RigidTimerHandle));
                              }),
                              TimerRemaining
                              , false);
    OnRigid.Broadcast(TimerRemaining);

    ActivateBuff(0, 0.3, Causer);
    AttachedCharacter->GetCharacterMovement()->Deactivate();

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
    if (Buff->TryToActivateAction(AttachedCharacter))
    {
        ExclusiveGroupActionsMap.Add(Buff->GetActionExclusiveGroupID(), Buff);
        return true;
    }
    return false;
}

void UARPGCharacterCombatComponent::ReInitCharacterActions(
    UCharacterConfigPrimaryDataAsset* CharacterConfigPrimaryDataAsset)
{
    if (CharacterConfigPrimaryDataAsset)
    {
        MeleeAttackCollectionClasses = CharacterConfigPrimaryDataAsset->MeleeAttackCollectionClasses;
        RemoteAttackClasses = CharacterConfigPrimaryDataAsset->RemoteAttackClasses;
        AbilityClasses = CharacterConfigPrimaryDataAsset->AbilityClasses;
        BuffClasses = CharacterConfigPrimaryDataAsset->BuffClasses;

        MeleeAttackCollectionActions.Empty();
        RemoteAttackActions.Empty();
        AbilityActions.Empty();
        BuffActions.Empty();
    }

    SpawnActionActors(MeleeAttackCollectionClasses, MeleeAttackCollectionActions);
    SpawnActionActors(RemoteAttackClasses, RemoteAttackActions);
    SpawnActionActors(AbilityClasses, AbilityActions);
    SpawnActionActors(BuffClasses, BuffActions);

    CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
}
