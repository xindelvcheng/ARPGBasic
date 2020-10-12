// Fill out your copyright notice in the Description page of Project Settings.


#include "TranscendentalLawsSystem.h"
#include "ARPGAction.h"
#include "ARPGCharacter.h"
#include "ARPGCharacterCombatComponent.h"


void ATranscendentalLawsSystem::SetAttachedCharacter(AARPGCharacter* NewAttachedCharacter)
{
    AttachedCharacter = NewAttachedCharacter;
    AttachedCharacterCombatComponent = AttachedCharacter->GetCharacterCombatComponent();
    checkf(AttachedCharacter&&AttachedCharacterCombatComponent, TEXT("错误，角色不含有CharacterCombatComponent"))
}

void ATranscendentalLawsSystem::SetCurrentState(ETranscendentalLawsAttainment NewState)
{
    Attainment = NewState;
}

void ATranscendentalLawsSystem::Init(AARPGCharacter* NewAttachedCharacter,
                                     UARPGCharacterCombatComponent* NewAttachedCharacterCombatComponent)
{
    AttachedCharacter = NewAttachedCharacter;
    AttachedCharacterCombatComponent = NewAttachedCharacterCombatComponent;


    SpawnActionActors(MeleeAttackClasses, MeleeAttackCollectionActions);
    SpawnActionActors(RemoteAttackClasses, RemoteAttackActions);
    SpawnActionActors(AbilityClasses, AbilityActions);
    SpawnActionActors(BuffClasses, BuffActions);
}

void ATranscendentalLawsSystem::Activate()
{
    checkf(AttachedCharacter&&AttachedCharacterCombatComponent, TEXT("错误，激活仙道时未指定用户"))

    if (MeleeAttackCollectionActions.IsValidIndex(0))
    {
        AttachedCharacterCombatComponent->MeleeAttackCollectionActions.Emplace(MeleeAttackCollectionActions[0]);
        AttachedCharacterCombatComponent->CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
    }
}

void ATranscendentalLawsSystem::DeActivate()
{
    AttachedCharacterCombatComponent->MeleeAttackCollectionActions.RemoveAll([&](const AARPGAction* Action)->bool
    {
        return MeleeAttackCollectionActions.Contains(Action);
    });
    AttachedCharacterCombatComponent->CurrentMeleeAttackCollection = AttachedCharacterCombatComponent->MeleeAttackCollectionActions[0];
}

void ATranscendentalLawsSystem::BeginPlay()
{
    Super::BeginPlay();
}

void ATranscendentalLawsSystem::SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& ActionClasses,
                                                  TArray<AARPGAction*>& ActionActors)
{
    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    FTransform Transform;
    for (auto ActionClass : ActionClasses)
    {
        AARPGAction* Action = Cast<AARPGAction>(GetWorld()->SpawnActor(ActionClass, &Transform, ActorSpawnParameters));
        check(Action);
        Action->InitWithOwningCharacter(AttachedCharacter);
        ActionActors.Add(Action);
        Action->OnActionFinished.BindUObject(AttachedCharacterCombatComponent,
                                             &UARPGCharacterCombatComponent::BindToOnActionFinished);
    }
}
