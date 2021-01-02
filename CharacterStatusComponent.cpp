// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusComponent.h"


#include "ArchiveStructs.h"
#include "ARPGBasicSettings.h"
#include "CharacterConfigPrimaryDataAsset.h"
#include "ARPGCharacter.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGMainCharacter.h"
#include "ARPGSpecialEffectsSubsystem.h"


void UCharacterStatusComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ReInitCharacterProperties();
}

void UCharacterStatusComponent::ReInitCharacterProperties(UCharacterConfigPrimaryDataAsset* CharacterConfigDataAsset)
{
	if (CharacterConfigDataAsset)
	{
		Level = CharacterConfigDataAsset->Level;
		SetCoins(CharacterConfigDataAsset->Coins);

		HealthSpecialty = CharacterConfigDataAsset->HealthSpecialty;
		StaminaSpecialty = CharacterConfigDataAsset->StaminaSpecialty;
		AttackSpecialty = CharacterConfigDataAsset->AttackSpecialty;
		DefenseSpecialty = CharacterConfigDataAsset->DefenseSpecialty;
		ToughnessSpecialty = CharacterConfigDataAsset->ToughnessSpecialty;

		DeathAnimation = CharacterConfigDataAsset->DeathAnimationAsset;
	}

	SetMaxHP(CalculateValueBySpecialty(HealthSpecialty));
	SetCurrentHP(MaxHP);
	SetMaxSP(CalculateValueBySpecialty(StaminaSpecialty));
	SetCurrentSP(MaxSP);

	Attack = CalculateValueBySpecialty(AttackSpecialty);
	Defense = CalculateValueBySpecialty(DefenseSpecialty);
	Toughness = CalculateValueBySpecialty(ToughnessSpecialty);
}

void UCharacterStatusComponent::SetCurrentHP(const int NewCurrentHP)
{
	if (CurrentHP <= 0)
	{
		return;
	}

	const int Temp = CurrentHP;
	CurrentHP = FMath::Clamp(NewCurrentHP, 0, MaxHP);
	OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentHP, CurrentHP, MaxHP, CurrentHP - Temp);
	if (this->CurrentHP <= 0)
	{
		OnCharacterDeath.Broadcast();
		if (GetOwnerCharacter())
		{
			GetOwnerCharacter()->SetCanBeDamaged(false);
			if (GetOwnerCharacter()->GetMesh())
			{
				GetOwnerCharacter()->GetMesh()->PlayAnimation(DeathAnimation, false);
			}


			if (AARPGMainCharacter* MainCharacter = UARPGCoreSubsystem::GetMainCharacter(GetWorld()))
			{
				if (GetOwnerCharacter() != MainCharacter)
				{
					MainCharacter->UpdateCoins(Coins);
					if (UARPGSpecialEffectsSubsystem* SpecialEffectsSubsystem = UARPGSpecialEffectsSubsystem::Get(
						GetWorld()))
					{
						SpecialEffectsSubsystem->PlaySoundEffect2D(TEXT("GetMoney"));
						UGameplayStatics::SpawnEmitterAtLocation(
							GetWorld(), SpecialEffectsSubsystem->GetParticleEffectResource(TEXT("Death")),
							GetOwnerCharacter()->GetActorLocation());
					}
				}
			}
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				GetWorld()->DestroyActor(GetOwner());
			}), 1, false);
		}
	}
}


void UCharacterStatusComponent::SetCurrentSP(const int NewCurrentSP)
{
	const int Temp = CurrentSP;
	CurrentSP = FMath::Clamp(NewCurrentSP, 0, MaxSP);
	OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentSP, CurrentSP, MaxSP, CurrentSP - Temp);
	if (this->CurrentSP <= 0)
	{
		OnCharacterStaminaExhausted.Broadcast();
	}
	FTimerManager& WorldTimerManager = GetWorld()->GetTimerManager();
	WorldTimerManager.ClearTimer(SPResumeTimerHandle);
	WorldTimerManager.ClearTimer(SPGrowingTimerHandle);

	if (!SPResumeTimerDelegate.IsBound())
	{
		SPResumeTimerDelegate.BindLambda([&]()
		{
			WorldTimerManager.SetTimer(SPGrowingTimerHandle, SPGrowingTimerDelegate, 0.1, true);
		});
	}

	if (!SPGrowingTimerDelegate.IsBound())
	{
		SPGrowingTimerDelegate.BindLambda([&]()
		{
			if (CurrentSP >= MaxSP)
			{
				WorldTimerManager.ClearTimer(SPGrowingTimerHandle);
				return;
			}
			const float FloatMaxSP = static_cast<float>(MaxSP);
			CurrentSP += static_cast<int>((FloatMaxSP / 20.f) * StaminaSpecialty) > 0
				             ? static_cast<int>((FloatMaxSP / 100.f) * StaminaSpecialty)
				             : 1;
			CurrentSP = CurrentSP > MaxSP ? MaxSP : CurrentSP;
			OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentSP, CurrentSP, MaxSP, CurrentSP - Temp);
		});
	}

	if (CurrentSP < MaxSP)
	{
		WorldTimerManager.SetTimer(SPResumeTimerHandle, SPResumeTimerDelegate, 3, false);
	}
}

// Sets default values for this component's properties
UCharacterStatusComponent::UCharacterStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

bool UCharacterStatusComponent::LevelUp(const ESpecialties Specialty)
{
	const float LevelUpCost = GetLevelUpCost();
	if (Coins >= LevelUpCost)
	{
		UpdateCharacterCoins(-LevelUpCost);

		switch (Specialty)
		{
		case ESpecialties::Health:
			HealthSpecialty++;
			break;
		case ESpecialties::Stamina:
			StaminaSpecialty++;
			break;
		case ESpecialties::Attack:
			AttackSpecialty ++;
			break;
		case ESpecialties::Defense:
			DefenseSpecialty ++;
			break;
		case ESpecialties::Toughness:
			ToughnessSpecialty ++;
		default:
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("无效的升级加点")); }
		}
		SetLevel(Level + 1);
		return true;
	}
	return false;
}
