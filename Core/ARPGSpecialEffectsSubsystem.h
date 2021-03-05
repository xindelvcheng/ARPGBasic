// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGActor.h"
#include "Components/BoxComponent.h"
#include "Engine/DataTable.h"
#include "Sound/SoundCue.h"
#include "particles/ParticleSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGSpecialEffectsSubsystem.generated.h"

class UARPGDamageBoxComponent;
class AARPGCharacter;

/*TODO:将AARPGSpecialEffectCreature分离为法术造物，并将其改为移动*/

USTRUCT()
struct FPlaySoundDescription
{
	GENERATED_BODY()

	FName SoundName;
	float VolumeMultiplier;
	float PitchMultiplier;
	float StartTime;
};

/**
 * 
 */
UCLASS()
class UARPGSpecialEffectsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	void LoadEffectsAssets();

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TMap<FName, USoundBase*> SoundEffects;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TMap<FName, UParticleSystem*> ParticleEffects;

	static UARPGSpecialEffectsSubsystem* Get(UWorld* World)
	{
		if (World && World->GetGameInstance())
		{
			return World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>();
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectsSubsystem")
	void PlaySoundEffect2D(FName EffectName);

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectsSubsystem")
	void PlaySpecialEffectAtLocation(FName EffectName, FVector Location);

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectsSubsystem")
	USoundBase* GetSoundEffectResource(FName EffectName)const { return SoundEffects.FindRef(EffectName); }

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectsSubsystem")
	UParticleSystem* GetParticleEffectResource(FName EffectName) const{ return ParticleEffects.FindRef(EffectName); }
};


/*开发阶段用于罗列项目中所有的粒子特效*/
USTRUCT(BlueprintType)
struct FParticleSystemListRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ParticleSystemList")
	UParticleSystem* ParticleSystem;
};
