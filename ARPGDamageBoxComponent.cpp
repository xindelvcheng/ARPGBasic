// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageBoxComponent.h"


#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGDamageSubsystem.h"

void UARPGDamageBoxComponent::SetDamageValue(float DamageWeightCoefficient)
{
	UnRegisterToDamageDetectIfRegistered();

	DetectDescription = {
		true, FVector{}, true,
		DamageWeight * DamageWeightCoefficient, DamageBias, VelocityDamageBonusWeight,
		DamageType
	};

	RegisterToDamageDetect();

	if (DamageWeightCoefficient > 1)
	{
		OnDamageIncrease(DamageWeightCoefficient - 1);
	}
	else
	{
		OnDamageDecrease(1 - DamageWeightCoefficient);
	}
}

void UARPGDamageBoxComponent::OnDamageIncrease(float DeltaDamageWeightCoefficient)
{
	UGameplayStatics::SpawnEmitterAttached(DamageIncreaseVFX, this);
}

void UARPGDamageBoxComponent::OnDamageDecrease(float DeltaDamageWeightCoefficient)
{

}

void UARPGDamageBoxComponent::RegisterToDamageDetect()
{
	if (AARPGCharacter* OwnerCharacter = Cast<AARPGCharacter>(GetOwner()))
	{
		if (UARPGDamageSubsystem* DamageSubsystem = UARPGDamageSubsystem::Get(OwnerCharacter->GetWorld()))
		{
			DamageDetectRecord = DamageSubsystem->RegisterToDamageDetect(
				this, OwnerCharacter, FDamageDetectedDelegate{}, DetectDescription);
			DamageDetectRecord->bDrawDebug = bDrawDebug;
		}
	}
}

void UARPGDamageBoxComponent::UnRegisterToDamageDetectIfRegistered()
{
	if (DamageDetectRecord)
	{
		if (UARPGDamageSubsystem* DamageSubsystem = UARPGDamageSubsystem::Get(GetWorld()))
		{
			DamageSubsystem->UnRegisterToDamageDetect(DamageDetectRecord);
		}
		DamageDetectRecord = nullptr;
	}
}

UARPGDamageBoxComponent::UARPGDamageBoxComponent()
{
	bAutoActivate = false;

	
}

void UARPGDamageBoxComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UARPGDamageBoxComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	UnRegisterToDamageDetectIfRegistered();
}

void UARPGDamageBoxComponent::ElementInteract(UARPGDamageBoxComponent* EnvironmentDamageBox)
{
	if (EnvironmentDamageBox->DamageType == UWoodDamage::StaticClass())
	{
		if (DamageType == UFireDamage::StaticClass())
		{
			SetDamageValue(1.5);
		}
		else if (DamageType == UWaterDamage::StaticClass())
		{
			SetDamageValue(0.5);
		}
	}
	else if (EnvironmentDamageBox->DamageType == UWaterDamage::StaticClass())
	{
		if (DamageType == UWoodDamage::StaticClass() || DamageType == UElectricityDamage::StaticClass())
		{
			SetDamageValue(1.5);
		}
		else if (DamageType == UFireDamage::StaticClass())
		{
			SetDamageValue(0.5);
		}
	}
	else if (EnvironmentDamageBox->DamageType == UMetalDamage::StaticClass())
	{
		if (DamageType == UElectricityDamage::StaticClass())
		{
			SetDamageValue(2);
		}
		else if (DamageType == UWoodDamage::StaticClass())
		{
			SetDamageValue(0.5);
		}
	}
	else if (EnvironmentDamageBox->DamageType == UFireDamage::StaticClass())
	{
		if (DamageType == UWoodDamage::StaticClass())
		{
			SetDamageValue(2);
		}
		else if (DamageType == UWaterDamage::StaticClass() || DamageType == UMetalDamage::StaticClass())
		{
			SetDamageValue(0.5);
		}
	}
	else if (EnvironmentDamageBox->DamageType == UStoneDamage::StaticClass())
	{
		if (DamageType == UWoodDamage::StaticClass())
		{
			SetDamageValue(1.5);
		}
		else if (DamageType == UWaterDamage::StaticClass())
		{
			SetDamageValue(0.5);
		}
	}
}

void UARPGDamageBoxComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	UnRegisterToDamageDetectIfRegistered();
	RegisterToDamageDetect();
}

void UARPGDamageBoxComponent::Deactivate()
{
	Super::Deactivate();

	UnRegisterToDamageDetectIfRegistered();
}
