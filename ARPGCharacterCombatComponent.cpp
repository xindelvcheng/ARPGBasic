// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterCombatComponent.h"
#include "ARPGCharacter.h"
#include "ARPGAction.h"
#include "CharacterConfigPrimaryDataAsset.h"



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

    RigidTimerDelegate.BindLambda([&]()
    {
        IsRigid = false;
        OnResumeFromRigid.Broadcast(GetWorld()->GetTimerManager().GetTimerElapsed(RigidTimerHandle));
    });

    ReInitCharacterActions();
}

void UARPGCharacterCombatComponent::BindToOnActionFinished()
{
    IsActing = false;
    CurrentActiveAction = nullptr;
}


// Called every frame
void UARPGCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...



}


bool UARPGCharacterCombatComponent::TryToMeleeAttack()
{
    if (CurrentActiveAction || IsRigid)
    {
        return false;
    }
    
    if (CurrentMeleeAttackCollection)
    {
        if (CurrentMeleeAttackCollection->CheckConditionAndPayCost())
        {
            CurrentMeleeAttackCollection->ActivateAction(AttachedCharacter);
            IsActing = true;
            CurrentActiveAction = CurrentMeleeAttackCollection;
            return true;
        }
    }
    return false;
}

bool UARPGCharacterCombatComponent::TryToRemoteAttack(int RemoteAttackIndex = 0)
{
    if (CurrentActiveAction || IsRigid)
    {
        return false;
    }

    if (RemoteAttackActions.IsValidIndex(RemoteAttackIndex))
    {
        AARPGAction* RemoteAttackAction = RemoteAttackActions[RemoteAttackIndex];
        if (RemoteAttackAction->CheckConditionAndPayCost())
        {
            RemoteAttackAction->ActivateAction(AttachedCharacter);
            IsActing = true;
            CurrentActiveAction = RemoteAttackAction;
            return true;
        }
    }
    return false;
}

bool UARPGCharacterCombatComponent::TryToUseAbility(int AbilityIndex = 0)
{
    if (CurrentActiveAction || IsRigid)
    {
        return false;
    }

    if (AbilityActions.IsValidIndex(AbilityIndex))
    {
        AARPGAction* Ability = AbilityActions[AbilityIndex];
        if (Ability->CheckConditionAndPayCost())
        {
            Ability->ActivateAction(AttachedCharacter);
            IsActing = true;
            CurrentActiveAction = Ability;
            return true;
        }
    }
    return false;
}

bool UARPGCharacterCombatComponent::CauseRigid(float Duration, AARPGCharacter* Causer = nullptr)
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
    return true;
}

void UARPGCharacterCombatComponent::ReInitCharacterActions(UCharacterConfigPrimaryDataAsset* CharacterConfigPrimaryDataAsset)
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
    
    SpawnActionActors(MeleeAttackCollectionClasses,MeleeAttackCollectionActions);
    SpawnActionActors(RemoteAttackClasses,RemoteAttackActions);
    SpawnActionActors(AbilityClasses,AbilityActions);
    SpawnActionActors(BuffClasses,BuffActions);

    CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
}


