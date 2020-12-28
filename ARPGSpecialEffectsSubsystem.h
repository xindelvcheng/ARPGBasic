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

UENUM(BlueprintType)
enum class EEffectCategory:uint8
{
	PositiveSoundEffects,
	NegativeSoundEffects,
	NeutralSoundEffects,
	PositiveVisualEffects,
	NegativeVisualEffects
};

/**
 * 
 */
UCLASS()
class UARPGSpecialEffectsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	template <typename T>
	void LoadEffectsAssets(TArray<T*>& Effects, TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TArray<USoundCue*> PositiveSoundEffects;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TArray<USoundCue*> NegativeSoundEffects;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TArray<USoundCue*> NeutralSoundEffects;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TArray<UParticleSystem*> PositiveVisualEffects;

	UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
	TArray<UParticleSystem*> NegativeVisualEffects;

	static UARPGSpecialEffectsSubsystem* Get(UWorld* World)
	{
		if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>())
		{
			return World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>();
		}

		return nullptr;
	}

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static void PlaySoundEffect2D(const UObject* WorldContextObject, EEffectCategory EffectCategory, int Index);

	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static void PlaySpecialEffectAtLocation(const UObject* WorldContextObject, EEffectCategory EffectCategory,
	                                        int Index, FVector Location);
};


USTRUCT(BlueprintType)
struct FARPGCreatureTimeLineTaskStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageStartTime = 0.3;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float Duration = 0.2;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	USoundBase* SoundEffectAsset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	UParticleSystem* VisualEffectAsset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	FTransform VisualEffectRelativeTransform;

	TWeakObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	TWeakObjectPtr<UAudioComponent> AudioComponent;

	float DamageEndTime;

	UPROPERTY()
	class UDamageDetectRecord* DamageDetectRecord;
};

UCLASS(Blueprintable)
class AARPGSpecialEffectCreature : public AARPGActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	UParticleSystem* DamageIncreaseVFX;

protected:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	UARPGDamageBoxComponent* DamageDetectionBox;

	/*该组件仅在编译器模式下被初始化，用于调试粒子系统匹配伤害盒子*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    UParticleSystemComponent* DebugParticleSystem;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageWeight = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageBias = 10;


	virtual void BeginPlay() override;


public:
	AARPGSpecialEffectCreature();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TArray<FARPGCreatureTimeLineTaskStruct> TimeLineTasks;


	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectCreature",DisplayName="CreateARPGSpecialEffectCreature")
	static AARPGSpecialEffectCreature* Create(TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
	                                          FTransform Transform, AARPGCharacter* CreatureOwnerCharacter);


	virtual UParticleSystem* GetDamageIncreaseVFX()
	{
		return DamageIncreaseVFX;
	}

	virtual void SetDamageIncreaseVFX(UParticleSystem* CreatureDamageIncreaseVfx)
	{
		DamageIncreaseVFX = CreatureDamageIncreaseVfx;
	}
};

/*用于罗列项目中所有的粒子特效*/
USTRUCT(BlueprintType)
struct FParticleSystemListRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ParticleSystemList")
	UParticleSystem* ParticleSystem;
};