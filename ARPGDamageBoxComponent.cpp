// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageBoxComponent.h"


#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGDamageTypes.h"


void UARPGDamageBoxComponent::SetDamageValue(float DamageWeightCoefficient)
{
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

UARPGDamageBoxComponent::UARPGDamageBoxComponent()
{
	bAutoActivate = false;
	PrimaryComponentTick.bCanEverTick = true;
}

void UARPGDamageBoxComponent::EnableDamageDetected()
{
	Timer = 0;
	LastFrameLocation = GetComponentLocation();
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UARPGDamageBoxComponent::DisableDamageDetected()
{
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UARPGDamageBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	AARPGCharacter* OwnerCharacter = GetOwner<AARPGCharacter>();
	ActorsToIgnore.AddUnique(OwnerCharacter);
	
	PrimaryComponentTick.SetTickFunctionEnable(false);
}


void UARPGDamageBoxComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timer += DeltaTime;
	CurrentFrameLocation = GetComponentLocation();

	if (DamageDetectDescriptionStruct.bUseDamageCenterComponentCurrentBoundsAsDetectBound)
	{
		const FBoxSphereBounds BoxBounds = CalcLocalBounds();
		DamageDetectDescriptionStruct.DamageBoxHalfSizeInTrace = BoxBounds.BoxExtent *
			GetComponentScale();
	}

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), LastFrameLocation, CurrentFrameLocation,
	                                              DamageDetectDescriptionStruct.DamageBoxHalfSizeInTrace,
	                                              GetComponentRotation(),
	                                              DetectObjectTypes,
	                                              false,
	                                              ActorsToIgnore,
	                                              bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	                                              HitResults,
	                                              true);

	for (FHitResult HitResult : HitResults)
	{
		if (HitResult.GetComponent() != this)
		{
			OnHitDetected(HitResult);
		}
	}

	LastFrameLocation = CurrentFrameLocation;
}

void UARPGDamageBoxComponent::OnHitDetected(FHitResult HitResult)
{
	HitDetectedEvent.Broadcast(HitResult);
	float BaseAttack = 1;
	AARPGCharacter* OwnerCharacter = GetOwner<AARPGCharacter>();

	if (OwnerCharacter)
	{
		BaseAttack = OwnerCharacter->GetCharacterStatusComponent()->GetAttack();
	}


	const float BaseDamage = DamageDetectDescriptionStruct.DamageWeight * BaseAttack + DamageDetectDescriptionStruct.
		DamageBias + GetOwner()->GetVelocity().Size() *
		DamageDetectDescriptionStruct.VelocityDamageBonusWeight;

	if (UARPGDamageBoxComponent* DamageBoxComponent = Cast<UARPGDamageBoxComponent>(HitResult.GetComponent()))
	{
		ElementInteract(DamageBoxComponent);
	}

	if (DamageDetectDescriptionStruct.CauseDamage)
	{
		AActor* HitActor = HitResult.GetActor();
		UGameplayStatics::ApplyPointDamage(
			HitActor, BaseDamage,
			HitResult.Location, HitResult,
			OwnerCharacter ? OwnerCharacter->GetController() : nullptr,
			OwnerCharacter, DamageDetectDescriptionStruct.DamageTypeClass);
	}
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
