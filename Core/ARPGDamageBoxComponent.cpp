// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageBoxComponent.h"


#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGDamageTypes.h"
#include "ARPGStaticFunctions.h"


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
	ActorDamagedInSingleAttack.Empty();
}

void UARPGDamageBoxComponent::DisableDamageDetected()
{
	ActorDamagedInSingleAttack.Empty();
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UARPGDamageBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	PrimaryComponentTick.SetTickFunctionEnable(false);

	AARPGCharacter* OwnerCharacter = GetOwner<AARPGCharacter>();
	ActorsToIgnore.AddUnique(OwnerCharacter);
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


#pragma optimize("",off)
void UARPGDamageBoxComponent::OnHitDetected(FHitResult HitResult)
{
	HitDetectedDelegate.Broadcast(HitResult);
	DamageDetectedEvent.Broadcast(HitResult);

	if (UARPGDamageBoxComponent* DamageBoxComponent = Cast<UARPGDamageBoxComponent>(HitResult.GetComponent()))
	{
		ElementInteract(DamageBoxComponent);
	}
	if (DamageDetectDescriptionStruct.bCauseDamage)
	{
		CauseDamage(HitResult);
	}
}


void UARPGDamageBoxComponent::CauseDamage(FHitResult HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	if (ActorDamagedInSingleAttack.Contains(HitActor))
	{
		return;
	}

	float BaseDamage = DamageDetectDescriptionStruct.DamageBias;
	AController* DamageInstigatorController = nullptr;
	if (AARPGCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		const float BaseAttack = OwnerCharacter->GetCharacterStatusComponent()->GetAttack();
		BaseDamage = DamageDetectDescriptionStruct.DamageWeight * BaseAttack +
			DamageDetectDescriptionStruct.DamageBias + GetOwner()->GetVelocity().Size() *
			DamageDetectDescriptionStruct.VelocityDamageBonusWeight;
		DamageInstigatorController = OwnerCharacter->GetController();
	}
	UGameplayStatics::ApplyPointDamage(HitActor, BaseDamage,
	                                   HitResult.Location, HitResult,
	                                   DamageInstigatorController,
	                                   GetOwner(), DamageDetectDescriptionStruct.DamageTypeClass);
	ActorDamagedInSingleAttack.Add(HitActor);
}
#pragma optimize("",on)

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


void UARPGDamageBoxComponent::SetDebugEnable(bool bShowDebug)
{
	bDrawDebug = bShowDebug;
}

AARPGCharacter* UARPGDamageBoxComponent::GetOwnerCharacter() const
{
	AActor* Owner = GetOwner();
	while (Owner)
	{
		if (AARPGCharacter* GetOwnerCharacter = Cast<AARPGCharacter>(Owner))
		{
			return GetOwnerCharacter;
		}
		Owner = Owner->GetOwner();
	}
	return nullptr;
}
