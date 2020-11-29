// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPGCharacter.h"

#include "GameFramework/Character.h"
#include "particles/ParticleSystem.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "ARPGLockTargetComponent.h"
#include "ARPGAIPerceptionStimuliSourceComponent.h"
#include "ARPGBasicSettings.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
		if (!ImpactSoundEffect)
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

	RefreshWithCharacterConfigPDataAsset();
}

float AARPGCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UpdateCurrentHP(-DamageAmount);

	FVector HitLocation = GetActorLocation();
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		HitLocation = PointDamageEvent->HitInfo.Location;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVisualEffect, HitLocation);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSoundEffect, HitLocation);
	}

	if (HitReactAnimMontages.Num() > 0)
	{
		PlayAnimMontage(
			HitReactAnimMontages.IsValidIndex(HitReactIndex) ? HitReactAnimMontages[HitReactIndex] : nullptr);
		HitReactIndex = (HitReactIndex + 1) % HitReactAnimMontages.Num();
		FTimerManager& WorldTimerManager = GetWorldTimerManager();
		WorldTimerManager.ClearTimer(ResetHitReactIndexTimerHandle);
		WorldTimerManager.SetTimer(ResetHitReactIndexTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			HitReactIndex = 0;
		}), 2, false);
	}

	const float ImpactForceFactor = 1000 * (DamageAmount / GetMaxHP());
	LaunchCharacter(
		(UKismetMathLibrary::GetDirectionUnitVector(
			EventInstigator ? EventInstigator->GetTargetLocation() : HitLocation,
			GetActorLocation()) * ImpactForceFactor),
		true, true);
	CauseRigid(1 * (DamageAmount / GetMaxHP()), nullptr);
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

void AARPGCharacter::RefreshWithCharacterConfigPDataAsset()
{
	if (CharacterConfigPDataAsset)
	{
		CharacterName = CharacterConfigPDataAsset->CharacterName;

		CharacterName = CharacterConfigPDataAsset->CharacterName;
		CharacterStatusComponent->ReInitCharacterProperties(CharacterConfigPDataAsset);
		CharacterCombatComponent->ReInitCharacterActions(CharacterConfigPDataAsset);
		ReInitCharacterArtResources(CharacterConfigPDataAsset);
	}
	else
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(
			FString::Printf(TEXT("角色%s未设置CharacterConfigPDataAsset"), *GetName()));
	}
}

void AARPGCharacter::ReInitCharacterArtResources(UCharacterConfigPrimaryDataAsset* CharacterConfigDataAsset)
{
	HitReactAnimMontages = CharacterConfigPDataAsset->HitReactAnimMontages;
	FootstepSoundEffects = CharacterConfigPDataAsset->FootstepSoundEffects;
}


void AARPGCharacter::PlayFootStepSoundEffect(EGroundTypeEnum GroundType, float Volume)
{
	const auto FootstepSoundEffect = FootstepSoundEffects.FindByPredicate([=](FGroundTypeFootstepSoundPairStruct Pair)
	{
		return Pair.GroundType == GroundType;
	})->FootstepSoundEffect;
	FVector FootstepPlayLocation;
	if (GetMesh() && GetMesh()->GetSocketLocation(TEXT("Root")) != FVector{0, 0, 0})
	{
		FootstepPlayLocation = GetMesh()->GetSocketLocation(TEXT("Root"));
	}
	else if (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() > 0)
	{
		FootstepPlayLocation = GetActorLocation() + FVector{0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()};
	}
	else
	{
		FootstepPlayLocation = GetActorLocation() + FVector{0, 0, -50};
	}
	UGameplayStatics::PlaySoundAtLocation(this, FootstepSoundEffect, FootstepPlayLocation, Volume);
}
