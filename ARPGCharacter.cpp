// Fill out your copyright notice in the Description page of Project Settings.

#include "particles/ParticleSystem.h"

#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "ARPGLockTargetComponent.h"
#include "ARPGAIPerceptionStimuliSourceComponent.h"
#include "ARPGBasicSettings.h"


AARPGCharacter::AARPGCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    CharacterStatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>("CharacterStatusComponent");
    CharacterCombatComponent = CreateDefaultSubobject<UTranscendentalCombatComponent>("ARPGCharacterCombaComponent");
    CharacterLockTargetComponent = CreateDefaultSubobject<UARPGLockTargetComponent>("ARPGLockTargetComponent");
    AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UARPGAIPerceptionStimuliSourceComponent>(
        "AIPerceptionStimuliSourceComponent");

    if (UARPGBasicSettings::Get())
    {
        if (!ImpactVisualEffect)
        {
            ImpactVisualEffect = UARPGBasicSettings::Get()->DefaultImpactVisualEffect.LoadSynchronous();
        }
        if(!ImpactSoundEffect)
        {
            ImpactSoundEffect = UARPGBasicSettings::Get()->DefaultImpactSoundEffect.LoadSynchronous();
        }
    }
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
        HitReactAnimMontages = CharacterConfigPDataAsset->HitReactAnimMontages;
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

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointDamageEvent= static_cast<const FPointDamageEvent*>(&DamageEvent);
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactVisualEffect,PointDamageEvent->HitInfo.Location);
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(),ImpactSoundEffect,PointDamageEvent->HitInfo.Location);
    }

    if (HitReactAnimMontages.Num()>0)
    {
        PlayAnimMontage(HitReactAnimMontages.IsValidIndex(HitReactIndex) ? HitReactAnimMontages[HitReactIndex] : nullptr);
        HitReactIndex = (HitReactIndex + 1) % HitReactAnimMontages.Num();
        FTimerManager& WorldTimerManager = GetWorldTimerManager();
        WorldTimerManager.ClearTimer(ResetHitReactIndexTimerHandle);
        WorldTimerManager.SetTimer(ResetHitReactIndexTimerHandle,FTimerDelegate::CreateLambda([&]()
        {
            HitReactIndex = 0;
        }),2,false);
    }
    
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
