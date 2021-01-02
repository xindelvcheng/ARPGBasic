// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPGCharacter.h"

#include "GameFramework/Character.h"
#include "particles/ParticleSystem.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGCollectionComponent.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "ARPGLockTargetComponent.h"
#include "ARPGAIPerceptionStimuliSourceComponent.h"
#include "ARPGBasicSettings.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include <cmath>

#include "ARPGAimComponent.h"
#include "ARPGStaticFunctions.h"


AARPGCharacter::AARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CharacterStatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>("CharacterStatusComponent");
	CharacterCombatComponent = CreateDefaultSubobject<UTranscendentalCombatComponent>("ARPGCharacterCombaComponent");
	
	AIPerceptionStimuliSourceComponent = CreateDefaultSubobject<UARPGAIPerceptionStimuliSourceComponent>(
		"AIPerceptionStimuliSourceComponent");
	
	GameItemsManagerComponent = CreateDefaultSubobject<UARPGBagComponent>("GameItemsManagerComponent");
	SpellsManagerComponent = CreateDefaultSubobject<UARPGSpellsManagerComponent>("SpellsManagerComponent");

	CharacterLockTargetComponent = CreateDefaultSubobject<UARPGLockTargetComponent>("ARPGLockTargetComponent");
	CharacterLockTargetComponent->SetupAttachment(RootComponent);
	
	AimComponent = CreateDefaultSubobject<UARPGAimComponent>(TEXT("AimComponent"));
	AimComponent->SetupAttachment(RootComponent);

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
	if (CharacterConfigDataAsset)
	{
		return CharacterConfigDataAsset->CharacterDisplayName;
	}
	return FText();
}


void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AARPGCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	LoadCharacterConfigDataAsset();
}


void AARPGCharacter::LoadCharacterConfigDataAsset()
{
	if (CharacterConfigDataAsset)
	{
		CharacterName = CharacterConfigDataAsset->CharacterName;
		CharacterDisplayName = CharacterConfigDataAsset->CharacterDisplayName;
		HitReactAnimMontages = CharacterConfigDataAsset->HitReactAnimMontages;
		FootstepSoundEffects = CharacterConfigDataAsset->FootstepSoundEffects;
	}
	else
	{
		UARPGStaticFunctions::PrintLogToScreen(
            FString::Printf(TEXT("角色%s未设置CharacterConfigPDataAsset"), *GetName()));
	}
}

void AARPGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

float AARPGCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	//同招不可互接机制
	if (int& DamageTypeCounter = DamageToleranceCounter.FindOrAdd(DamageEvent.DamageTypeClass,0))
	{
		ActualDamage *= pow(2, -(DamageTypeCounter++));

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this,DamageEvent]()
		{
			if (this)
			{
				DamageToleranceCounter.Add(DamageEvent.DamageTypeClass,
				                           DamageToleranceCounter.FindRef(DamageEvent.DamageTypeClass) - 1);
			}
		}), 5, false);
	}
	UpdateCurrentHP(-ActualDamage);


	//受伤特效
	FVector HitLocation = GetActorLocation();
	
	if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		HitLocation = PointDamageEvent->HitInfo.Location;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVisualEffect, HitLocation);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSoundEffect, HitLocation);
	}

	//受伤动作反馈
	if (HitReactAnimMontages.Num() > 0)
	{
		PlayAnimMontage(
			HitReactAnimMontages.IsValidIndex(HitReactIndex) ? HitReactAnimMontages[HitReactIndex] : nullptr);
		HitReactIndex = (HitReactIndex + 1) % HitReactAnimMontages.Num();
		FTimerManager& WorldTimerManager = GetWorldTimerManager();
		WorldTimerManager.SetTimer(ResetHitReactIndexTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			HitReactIndex = 0;
		}), 2, false);
	}

	//冲击力
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

void AARPGCharacter::AddToSpellPanel(int index)
{
	SpellsManagerComponent->AddItem(CharacterCombatComponent->GetAbilityActions()[index]);
}
