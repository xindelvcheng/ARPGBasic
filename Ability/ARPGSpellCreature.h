// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGActor.h"
#include "ARPGDamageBoxComponent.h"


#include "ARPGSpellCreature.generated.h"

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
};

/**
 * 
 */
UCLASS()
class TESTPROJECT_API AARPGSpellCreature : public AARPGActor
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	UParticleSystem* DamageIncreaseVFX;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	UARPGDamageBoxComponent* DamageDetectionBox;

	/*该组件仅用于调试粒子系统匹配伤害盒子*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	UParticleSystemComponent* DebugParticleSystem;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	float DamageWeight = 1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	float DamageBias = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	TArray<FARPGCreatureTimeLineTaskStruct> TimeLineTasks;

protected:
	virtual void BeginPlay() override;

public:

	AARPGSpellCreature();
	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectCreature",DisplayName="CreateARPGSpecialEffectCreature")
    static AARPGSpellCreature* Create(TSubclassOf<AARPGSpellCreature> CreatureClass,
                                              FTransform Transform, AARPGCharacter* CreatureOwnerCharacter);
};
