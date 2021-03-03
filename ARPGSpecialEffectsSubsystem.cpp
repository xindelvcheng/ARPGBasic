// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpecialEffectsSubsystem.h"

#include "particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"

#include "ARPGBasicSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ARPGCharacter.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"


void UARPGSpecialEffectsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadEffectsAssets();
}

void UARPGSpecialEffectsSubsystem::LoadEffectsAssets()
{
	if (const auto ARPGBasicSettings = UARPGBasicSettings::Get())
	{
		for (auto NameSoundPair : ARPGBasicSettings->SoundEffects)
		{
			SoundEffects.Add(NameSoundPair.Key, NameSoundPair.Value.LoadSynchronous());
		}
		for (auto NameParticlePair : ARPGBasicSettings->ParticleEffects)
		{
			ParticleEffects.Add(NameParticlePair.Key, NameParticlePair.Value.LoadSynchronous());
		}
	}
}

void UARPGSpecialEffectsSubsystem::PlaySoundEffect2D(FName EffectName)
{
	if (USoundBase* SoundResource = SoundEffects.FindRef(EffectName))
	{
		UGameplayStatics::PlaySound2D(this, SoundResource);
	}
	else
	{
		UARPGStaticFunctions::PrintLogToScreen(TEXT("UARPGSpecialEffectsSubsystem:PlaySoundEffect2D未找到指定音乐资源"));
	}
}

void UARPGSpecialEffectsSubsystem::PlaySpecialEffectAtLocation(FName EffectName, FVector Location)
{
	if (USoundBase* SoundResource = SoundEffects.FindRef(EffectName))
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundResource, Location);
	}
	else
	{
		UARPGStaticFunctions::PrintLogToScreen(
			TEXT("UARPGSpecialEffectsSubsystem:PlaySpecialEffectAtLocation未找到指定音乐资源"));
	}
}


