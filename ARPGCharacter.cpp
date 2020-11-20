// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacter.h"

#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "ARPGLockTargetComponent.h"
#include "ARPGAIPerceptionStimuliSourceComponent.h"

AARPGCharacter::AARPGCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    CharacterStatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>("CharacterStatusComponent");
    CharacterCombatComponent = CreateDefaultSubobject<UTranscendentalCombatComponent>("ARPGCharacterCombaComponent");
    CharacterLockTargetComponent = CreateDefaultSubobject<UARPGLockTargetComponent>("ARPGLockTargetComponent");
    AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UARPGAIPerceptionStimuliSourceComponent>("AIPerceptionStimuliSourceComponent");
}

FText AARPGCharacter::GetCharacterDisplayName() const
{
    if (CharacterConfigPDataAsset)
    {
        return CharacterConfigPDataAsset->CharacterDisplayName;
    }
    return FText();
}

void AARPGCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (CharacterConfigPDataAsset)
    {
        CharacterStatusComponent->ReInitCharacterProperties(CharacterConfigPDataAsset);
        CharacterCombatComponent->ReInitCharacterActions(CharacterConfigPDataAsset);
        CharacterName = CharacterConfigPDataAsset->CharacterName;
    }
    else
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(
            FString::Printf(TEXT("角色%s未设置CharacterConfigPDataAsset"), *GetName()));
    }
}

float AARPGCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
    AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    UpdateCurrentHP(-DamageAmount);
    return ActualDamage;
}

void AARPGCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
