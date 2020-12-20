// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
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

	TWeakObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	TWeakObjectPtr<UAudioComponent> AudioComponent;

	float DamageEndTime;

	UPROPERTY()
	class UDamageDetectRecord* DamageDetectRecord;
};

UCLASS(Blueprintable)
class AARPGSpecialEffectCreature : public AActor
{
	GENERATED_BODY()

protected:
	TWeakObjectPtr<AARPGCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	UARPGDamageBoxComponent* DamageDetectionBox;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageWeight = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageBias = 10;

	virtual void BeginPlay() override;


public:
	AARPGSpecialEffectCreature();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TSubclassOf<UDamageType> DamageType;

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffect")
	void SetOwnerCharacter(AARPGCharacter* NewOwnerCharacter) { OwnerCharacter = NewOwnerCharacter; }

	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffect")
	AARPGCharacter* GetOwnerCharacter() const { return OwnerCharacter.Get(); }

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TArray<FARPGCreatureTimeLineTaskStruct> TimeLineTasks;


	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectCreature",DisplayName="CreateARPGSpecialEffectCreature")
	static AARPGSpecialEffectCreature* Create(TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
	                                          FTransform Transform, AARPGCharacter* OwnerCharacter);
};
