// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageBoxComponent.h"

#include "ARPGCharacter.h"
#include "ARPGDamageSubsystem.h"

void UARPGDamageBoxComponent::SetDamageValue(float DamageWeightCoefficient, float DeltaDamageBias = 0)
{
	UnRegisterToDamageDetectIfRegistered();
	
	DetectDescription = {
		true, FVector{}, true,
		DamageWeight * DamageWeightCoefficient, DamageBias + DeltaDamageBias, VelocityDamageBonusWeight,
		DamageType
	};
	RegisterToDamageDetect();
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

	SetDamageValue(1);
}

void UARPGDamageBoxComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UARPGDamageBoxComponent::ElementInteract(UARPGDamageBoxComponent* OtherDamageBox)
{
	if (this->DamageType == UFireDamage::StaticClass() && OtherDamageBox->DamageType == UWoodDamage::StaticClass())
	{
		this->DamageType = nullptr;
		SetDamageValue(1.5);
	}

	if (this->DamageType == UFireDamage::StaticClass() && OtherDamageBox->DamageType == UWaterDamage::StaticClass())
	{
		this->DamageType = nullptr;
		SetDamageValue(0.5);
	}

	if (this->DamageType == UElectricityDamage::StaticClass() && OtherDamageBox->DamageType ==
		UWaterDamage::StaticClass())
	{
		SetDamageValue(1.5);
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
