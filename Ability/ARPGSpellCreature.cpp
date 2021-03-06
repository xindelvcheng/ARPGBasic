// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpellCreature.h"


#include "ARPGBasicSettings.h"
#include "ARPGStaticFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"


AARPGSpellCreature::AARPGSpellCreature()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootSceneComponent"));

	DamageDetectionBox = CreateDefaultSubobject<UARPGDamageBoxComponent>(TEXT("DamageDetectionBox"));
	DamageDetectionBox->SetupAttachment(RootComponent);

	ActorMovementComponent = CreateDefaultSubobject<UARPGActorMovementComponent>("ActorMovementComponent");

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

void AARPGSpellCreature::BeginPlay()
{
	Super::BeginPlay();

	for (FARPGCreatureTimeLineTaskDescription& TaskDescription : TimeLineTasks)
	{
		FTimerHandle StartTimerHandle;

		/*若计时器的延时为0，则不会执行（而不是立即执行）*/
		if (TaskDescription.StartTime > 0)
		{
			GetWorldTimerManager().SetTimer(StartTimerHandle, FTimerDelegate::CreateLambda([&]()
			{
				ExecuteTask(TaskDescription);
			}), TaskDescription.StartTime, false);
		}
		else
		{
			ExecuteTask(TaskDescription);
		}

		FTimerHandle EndTimerHandle;
		GetWorldTimerManager().SetTimer(EndTimerHandle, FTimerDelegate::CreateLambda([&]()
		{
			FinishTask(TaskDescription);
		}), TaskDescription.StartTime + TaskDescription.Duration, false);
	}
}

void AARPGSpellCreature::ExecuteTask(FARPGCreatureTimeLineTaskDescription& TaskDescription)
{
	//显示特效
	TaskDescription.ParticleSystemComponent = UGameplayStatics::SpawnEmitterAttached(
		TaskDescription.VisualEffectAsset, RootComponent);
	if (TaskDescription.ParticleSystemComponent.IsValid())
	{
		TaskDescription.ParticleSystemComponent->
		                SetRelativeTransform(TaskDescription.VisualEffectRelativeTransform);
	}
	TaskDescription.AudioComponent = UGameplayStatics::SpawnSoundAttached(
		TaskDescription.SoundEffectAsset, RootComponent);

	/*开启伤害*/
	if (TaskDescription.bCauseDamage)
	{
		DamageDetectionBox->EnableDamageDetected();
	}

	/*移动*/
	if (TaskDescription.MoveModeEnum == EMoveModeEnum::Forward)
	{
		TaskDescription.TickerDelegateHandle = FTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateLambda([&](float DeltaTime)-> bool
			{
				AddActorLocalOffset(FVector{100, 0, 0} * DeltaTime);
				return true;
			}));
	}
}

void AARPGSpellCreature::FinishTask(FARPGCreatureTimeLineTaskDescription& TaskDescription)
{
	if (TaskDescription.ParticleSystemComponent.IsValid())
	{
		TaskDescription.ParticleSystemComponent->DestroyComponent();
	}

	if (TaskDescription.AudioComponent.IsValid())
	{
		TaskDescription.AudioComponent->DestroyComponent();
	}

	if (TaskDescription.bCauseDamage)
	{
		DamageDetectionBox->DisableDamageDetected();
	}

	FTicker::GetCoreTicker().RemoveTicker(TaskDescription.TickerDelegateHandle);
}


AARPGSpellCreature* AARPGSpellCreature::Create(TSubclassOf<AARPGSpellCreature> CreatureClass, FTransform Transform,
                                               AARPGCharacter* CreatureOwnerCharacter)
{
	if (const auto SpecialEffectCreature = UARPGStaticFunctions::SpawnActor<AARPGSpellCreature>(
		CreatureClass, Transform, CreatureOwnerCharacter))
	{
		return SpecialEffectCreature;
	}

	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("ASpellawCreature初始化出现错误")));
	return nullptr;
}
