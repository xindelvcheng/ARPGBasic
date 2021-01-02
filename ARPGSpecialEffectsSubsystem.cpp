// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpecialEffectsSubsystem.h"

#include "particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"

#include "ARPGBasicSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ARPGCharacter.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGDamageSubsystem.h"
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


AARPGSpecialEffectCreature::AARPGSpecialEffectCreature()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootSceneComponent"));

	DamageDetectionBox = CreateDefaultSubobject<UARPGDamageBoxComponent>(TEXT("DamageDetectionBox"));
	DamageDetectionBox->SetupAttachment(RootComponent);

	if (!DamageIncreaseVFX)
	{
		if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
		{
			DamageIncreaseVFX = BasicSettings->DefaultDamageIncreaseVFX.LoadSynchronous();
			DamageDetectionBox->SetDamageIncreaseVfx(DamageIncreaseVFX);
		}
	}

	if (DamageType != nullptr)
	{
		DamageDetectionBox->SetDamageType(DamageType);
	}

#if WITH_EDITOR
	DebugParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("DebugParticleSystem");
	DebugParticleSystem->SetupAttachment(RootComponent);
#endif
}

void AARPGSpecialEffectCreature::BeginPlay()
{
	Super::BeginPlay();

	for (FARPGCreatureTimeLineTaskStruct& Task : TimeLineTasks)
	{
		FTimerHandle StartTimerHandle;

		//显示特效
		Task.ParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
			Task.VisualEffectAsset, RootComponent);
		Task.ParticleSystemComponent->SetRelativeTransform(Task.VisualEffectRelativeTransform);
		Task.AudioComponent = UGameplayStatics::SpawnSoundAttached(Task.SoundEffectAsset, RootComponent);

		//进行伤害判定
		GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (UARPGDamageSubsystem* DamageSubsystem = UARPGDamageSubsystem::Get(GetWorld()))
			{
				Task.DamageDetectRecord = DamageSubsystem->RegisterToDamageDetect(
					DamageDetectionBox, GetOwnerCharacter(),
					FDamageDetectedDelegate{});
			}
		}), Task.DamageStartTime, false);

		//取消伤害判定
		FTimerHandle EndTimerHandle;
		Task.DamageEndTime = Task.DamageStartTime + Task.Duration;
		GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (UARPGDamageSubsystem* DamageSubsystem = UARPGDamageSubsystem::Get(GetWorld()))
			{
				DamageSubsystem->UnRegisterToDamageDetect(Task.DamageDetectRecord);
			}
		}), Task.DamageEndTime, false);
	}
}

AARPGSpecialEffectCreature* AARPGSpecialEffectCreature::Create(
	TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
	FTransform Transform,
	AARPGCharacter* CreatureOwnerCharacter)
{
	if (const auto SpecialEffectCreature = UARPGCoreSubsystem::SpawnActor<AARPGSpecialEffectCreature>(
		CreatureClass, Transform, CreatureOwnerCharacter))
	{
		return SpecialEffectCreature;
	}

	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("ARPGSpecialEffectCreature初始化出现错误")));
	return nullptr;
}
