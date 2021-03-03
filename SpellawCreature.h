// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"
#include "GameFramework/Actor.h"
#include "SpellawCreature.generated.h"

class UARPGDamageBoxComponent;
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

/*法术造物，Spellaw为自造词，意为法术*/
UCLASS(Blueprintable)
class ASpellawCreature : public AARPGActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
	UParticleSystem* DamageIncreaseVFX;

protected:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	UARPGDamageBoxComponent* DamageDetectionBox;

	/*该组件仅用于调试粒子系统匹配伤害盒子*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	UParticleSystemComponent* DebugParticleSystem;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageWeight = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	float DamageBias = 10;


	virtual void BeginPlay() override;


public:
	ASpellawCreature();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
	TArray<FARPGCreatureTimeLineTaskStruct> TimeLineTasks;


	UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectCreature",DisplayName="CreateARPGSpecialEffectCreature")
	static ASpellawCreature* Create(TSubclassOf<ASpellawCreature> CreatureClass,
	                                          FTransform Transform, AARPGCharacter* CreatureOwnerCharacter);


	virtual UParticleSystem* GetDamageIncreaseVFX()
	{
		return DamageIncreaseVFX;
	}

	virtual void SetDamageIncreaseVFX(UParticleSystem* CreatureDamageIncreaseVFX)
	{
		DamageIncreaseVFX = CreatureDamageIncreaseVFX;
	}
};
