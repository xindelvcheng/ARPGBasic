// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpecialEffectsSubsystem.h"

#include "particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"

#include "ARPGBasicSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ARPGCharacter.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGDamageSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"

template <typename T>
void UARPGSpecialEffectsSubsystem::LoadEffectsAssets(TArray<T*>& Effects,
                                                     TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs)
{
	for (TSoftObjectPtr<T> Effect : EffectSoftObjectPtrs)
	{
		Effects.Emplace(Effect.LoadSynchronous());
	}
}

void UARPGSpecialEffectsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UARPGBasicSettings::Get())
	{
		const auto ARPGBasicSettings = UARPGBasicSettings::Get();
		LoadEffectsAssets(PositiveSoundEffects, ARPGBasicSettings->PositiveSoundEffects);
		LoadEffectsAssets(NegativeSoundEffects, ARPGBasicSettings->NegativeSoundEffects);
		LoadEffectsAssets(NeutralSoundEffects, ARPGBasicSettings->NeutralSoundEffects);
		LoadEffectsAssets(PositiveVisualEffects, ARPGBasicSettings->PositiveVisualEffects);
		LoadEffectsAssets(NegativeVisualEffects, ARPGBasicSettings->NegativeVisualEffects);
	}
}

void UARPGSpecialEffectsSubsystem::PlaySoundEffect2D(const UObject* WorldContextObject, EEffectCategory EffectCategory,
                                                     int Index)
{
	if (UARPGSpecialEffectsSubsystem* ARPGSpecialEffectsSubsystemInstance = Get(WorldContextObject->GetWorld()))
	{
		switch (EffectCategory)
		{
		case EEffectCategory::PositiveSoundEffects:
			UGameplayStatics::PlaySound2D(WorldContextObject,
			                              ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects.IsValidIndex(Index)
				                              ? ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects[Index]
				                              : nullptr);
			break;
		case EEffectCategory::NegativeSoundEffects:
			UGameplayStatics::PlaySound2D(WorldContextObject,
			                              ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects.IsValidIndex(Index)
				                              ? ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects[Index]
				                              : nullptr);
			break;
		case EEffectCategory::NeutralSoundEffects:
			UGameplayStatics::PlaySound2D(WorldContextObject,
			                              ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects.IsValidIndex(Index)
				                              ? ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects[Index]
				                              : nullptr);
			break;
		default:
			UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，PlaySoundEffect2D函数只能播放音效"));
		}
	}
}


void UARPGSpecialEffectsSubsystem::PlaySpecialEffectAtLocation(const UObject* WorldContextObject,
                                                               EEffectCategory EffectCategory, int Index,
                                                               FVector Location)
{
	if (UARPGSpecialEffectsSubsystem* ARPGSpecialEffectsSubsystemInstance = Get(WorldContextObject->GetWorld()))
	{
		switch (EffectCategory)
		{
		case EEffectCategory::PositiveSoundEffects:
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
			                                      ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects.
			                                      IsValidIndex(Index)
				                                      ? ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects[Index]
				                                      : nullptr, Location);
			break;
		case EEffectCategory::NegativeSoundEffects:
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
			                                      ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects.
			                                      IsValidIndex(Index)
				                                      ? ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects[Index]
				                                      : nullptr, Location);
			break;
		case EEffectCategory::NeutralSoundEffects:
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
			                                      ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects.IsValidIndex(
				                                      Index)
				                                      ? ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects[Index]
				                                      : nullptr, Location);
			break;
		case EEffectCategory::PositiveVisualEffects:
			UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject,
			                                         ARPGSpecialEffectsSubsystemInstance->PositiveVisualEffects.
			                                         IsValidIndex(Index)
				                                         ? ARPGSpecialEffectsSubsystemInstance->PositiveVisualEffects[
					                                         Index]
				                                         : nullptr, Location);
			break;
		case EEffectCategory::NegativeVisualEffects:
			UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject,
			                                         ARPGSpecialEffectsSubsystemInstance->NegativeVisualEffects.
			                                         IsValidIndex(Index)
				                                         ? ARPGSpecialEffectsSubsystemInstance->NegativeVisualEffects[
					                                         Index]
				                                         : nullptr, Location);
			break;
		}
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
	if (const auto SpecialEffectCreature = UARPGGameInstanceSubsystem::SpawnActor<AARPGSpecialEffectCreature>(
		CreatureClass, Transform, CreatureOwnerCharacter))
	{
		return SpecialEffectCreature;
	}

	UARPGGameInstanceSubsystem::PrintLogToScreen(FString::Printf(TEXT("ARPGSpecialEffectCreature初始化出现错误")));
	return nullptr;
}
