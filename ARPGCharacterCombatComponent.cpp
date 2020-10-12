// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"




// Sets default values for this component's properties
UARPGCharacterCombatComponent::UARPGCharacterCombatComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}


void UARPGCharacterCombatComponent::SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& ActionClasses,
                                                      TArray<AARPGAction*>& ActionActors)
{
    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FTransform Transform;
    for (auto ActionClass : ActionClasses)
    {
        AARPGAction* Action = Cast<AARPGAction>(GetWorld()->SpawnActor(ActionClass,&Transform,ActorSpawnParameters));
        check(Action);
        Action->InitWithOwningCharacter(AttachedCharacter);
        ActionActors.Add(Action);
        Action->OnActionFinished.BindUObject(this, &UARPGCharacterCombatComponent::BindToOnActionFinished);
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

    RigidTimerDelegate.BindLambda([&]()
    {
        IsRigid = false;
        OnResumeFromRigid.Broadcast(GetWorld()->GetTimerManager().GetTimerElapsed(RigidTimerHandle));
    });

    SpawnActionActors(MeleeAttackCollectionClasses,MeleeAttackCollectionActions);
    SpawnActionActors(RemoteAttackClasses,RemoteAttackActions);
    SpawnActionActors(AbilityClasses,AbilityActions);
    SpawnActionActors(BuffClasses,BuffActions);

    CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
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


void UARPGCharacterCombatComponent::TryToMeleeAttack()
{
    if (IsActing || IsRigid)
    {
        return;
    }
    
    if (CurrentMeleeAttackCollection)
    {
        CurrentMeleeAttackCollection->ActivateAction(AttachedCharacter);
        IsActing = true;
        CurrentActiveAction = CurrentMeleeAttackCollection;
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


