// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellawCreature.h"

#include "ARPGBasicSettings.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGStaticFunctions.h"
#include "particles/ParticleSystemComponent.h"

ASpellawCreature::ASpellawCreature()
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
			DamageDetectionBox->SetDamageIncreaseVFX(DamageIncreaseVFX);
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

void ASpellawCreature::BeginPlay()
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

		//启用伤害判定
		GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			DamageDetectionBox->EnableDamageDetected();
		}), Task.DamageStartTime, false);

		//取消伤害判定
		FTimerHandle EndTimerHandle;
		Task.DamageEndTime = Task.DamageStartTime + Task.Duration;
		GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			DamageDetectionBox->DisableDamageDetected();
		}), Task.DamageEndTime, false);
	}
}

ASpellawCreature* ASpellawCreature::Create(
	TSubclassOf<ASpellawCreature> CreatureClass,
	FTransform Transform,
	AARPGCharacter* CreatureOwnerCharacter)
{
	if (const auto SpecialEffectCreature = UARPGStaticFunctions::SpawnActor<ASpellawCreature>(
		CreatureClass, Transform, CreatureOwnerCharacter))
	{
		return SpecialEffectCreature;
	}

	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("ASpellawCreature初始化出现错误")));
	return nullptr;
}
