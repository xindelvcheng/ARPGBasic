// Fill out your copyright notice in the Description page of Project Settings.


#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"

void UTranscendentalCombatComponent::BeginPlay()
{
    Super::BeginPlay();


    if (TranscendentalLawsSystemClasses.Num()>0)
    {
        FActorSpawnParameters ActorSpawnParameters;
        ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        for (auto TranscendentalLawsSystemClass : TranscendentalLawsSystemClasses)
        {
            FTransform Transform;
            auto TranscendentalLawsSystem = Cast<ATranscendentalLawsSystem>(
                GetWorld()->SpawnActor(TranscendentalLawsSystemClass, &Transform, ActorSpawnParameters));
            check(TranscendentalLawsSystem);
            TranscendentalLawsSystem->Init(AttachedCharacter,this);
            TranscendentalLawsSystems.Emplace(TranscendentalLawsSystem);
        }
    }
}

void UTranscendentalCombatComponent::ActivateTranscendentalLawsSystem(
    ATranscendentalLawsSystem* TranscendentalLawsSystem) 
{
    if (TranscendentalLawsSystems.Contains(TranscendentalLawsSystem))
    {
        TranscendentalLawsSystem->Activate();
        CurrentActiveTranscendentalLawsSystem = TranscendentalLawsSystem;
    }
}

void UTranscendentalCombatComponent::DeActivateTranscendentalLawsSystem(
    ATranscendentalLawsSystem* TranscendentalLawsSystem) 
{
    if (TranscendentalLawsSystems.Contains(TranscendentalLawsSystem))
    {
        TranscendentalLawsSystem->DeActivate();
        CurrentActiveTranscendentalLawsSystem = nullptr;
    }
}

void UTranscendentalCombatComponent::SwitchTranscendentalLawsSystem(ATranscendentalLawsSystem* TranscendentalLawsSystem)
{
    if (CurrentActiveTranscendentalLawsSystem)
    {
        CurrentActiveTranscendentalLawsSystem->DeActivate();
    }
    TranscendentalLawsSystem->Activate();
    CurrentActiveTranscendentalLawsSystem = TranscendentalLawsSystem;
}
