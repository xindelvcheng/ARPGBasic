// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGActor.h"
#include "ARPGDamageBoxComponent.h"


#include "ARPGSpellCreature.generated.h"

class UARPGActorMovementComponent;
UENUM()
enum class EMoveModeEnum : uint8 { NotMove, Forward, TowardTarget };

/**
 *用于快速配置ARPGSpellCreature的行为
 */
USTRUCT(BlueprintType)
struct FARPGCreatureTimeLineTaskDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	bool bCauseDamage = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	float StartTime = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	float Duration = 1.2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	USoundBase* SoundEffectAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	UParticleSystem* VisualEffectAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	FTransform VisualEffectRelativeTransform;

	TWeakObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	TWeakObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect")
	EMoveModeEnum MoveModeEnum;

	FDelegateHandle TickerDelegateHandle;
};

/**
 * 法术衍生物，负责播放法术的特效和产生伤害
 */
UCLASS()
class TESTPROJECT_API AARPGSpellCreature : public AARPGActor
{
	GENERATED_BODY()


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	UParticleSystem* DamageIncreaseVFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	UARPGDamageBoxComponent* DamageDetectionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	UARPGActorMovementComponent* ActorMovementComponent;

	/*该组件仅用于在编辑器中调试粒子系统匹配伤害盒子*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	UParticleSystemComponent* DebugParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	float DamageWeight = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	float DamageBias = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGSpecialEffect", meta=(AllowPrivateAccess))
	TArray<FARPGCreatureTimeLineTaskDescription> TimeLineTasks;

	void ExecuteTask(FARPGCreatureTimeLineTaskDescription& TaskDescription);

	void FinishTask(FARPGCreatureTimeLineTaskDescription& TaskDescription);

protected:
	virtual void BeginPlay() override;

public:

	AARPGSpellCreature();
	UFUNCTION(BlueprintCallable, Category="ARPGSpecialEffectCreature", DisplayName="CreateARPGSpecialEffectCreature")
	static AARPGSpellCreature* Create(TSubclassOf<AARPGSpellCreature> CreatureClass,
	                                  FTransform Transform, AARPGCharacter* CreatureOwnerCharacter);
};
