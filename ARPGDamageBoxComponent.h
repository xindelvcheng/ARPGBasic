// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "ARPGDamageBoxComponent.generated.h"

USTRUCT()
struct FDamageDetectDescriptionStruct
{
	GENERATED_BODY()

	bool bUseDamageCenterComponentCurrentBoundsAsDetectBound = true;
	/*DamageBoxHalfSizeInTrace在bUseDamageCenterComponentCurrentBoundsAsDetectBound = true（默认）时无效*/
	FVector DamageBoxHalfSizeInTrace = FVector(100, 100, 100);
	bool CauseDamage = true;
	float DamageWeight = 1;
	float DamageBias = 0;
	float VelocityDamageBonusWeight = 0.01;
	TSubclassOf<UDamageType> DamageTypeClass;
};


/**用于伤害检测，该组件的形状即为检测范围
 * 注意该组件不会自动激活，须在需要进行伤害检测时调用EnableDamageDetected()启动伤害判定，使用DisableDamageDetected()取消伤害判定
 */
UCLASS(ClassGroup=(ARPGBasic),meta=(BlueprintSpawnableComponent))
class UARPGDamageBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

	float Timer;

	FVector LastFrameLocation;
	FVector CurrentFrameLocation;

	TSubclassOf<UDamageType> DamageType;

	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObjectsInContainer
	TArray<AActor*> ActorsToIgnore;

	TArray<TEnumAsByte<EObjectTypeQuery>> DetectObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	bool bDrawDebug;

	FDamageDetectDescriptionStruct DamageDetectDescriptionStruct;

	UPROPERTY()
	UParticleSystem* DamageIncreaseVFX;

	DECLARE_MULTICAST_DELEGATE_OneParam(FDamageDetectedEvent, FHitResult);
	FDamageDetectedEvent HitDetectedEvent;

	virtual void OnHitDetected(FHitResult HitResult);

	void SetDamageValue(float DamageWeightCoefficient);

protected:
	virtual void OnDamageIncrease(float DeltaDamageWeightCoefficient);
	virtual void OnDamageDecrease(float DeltaDamageWeightCoefficient);

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	UARPGDamageBoxComponent();

	void EnableDamageDetected();
	void DisableDamageDetected();

	UFUNCTION(BlueprintCallable,Category="ARPGDamageBoxComponent")
	void SetDamageType(TSubclassOf<UDamageType> DefaultDamageType) { DamageType = DefaultDamageType; }

	/*和其他伤害类型的伤害盒子重叠时自己的反应（元素相生相克）*/
	void ElementInteract(UARPGDamageBoxComponent* OtherDamageBoxComponent);


	virtual FDamageDetectDescriptionStruct GetDetectDescription() const { return DamageDetectDescriptionStruct; }

	virtual void SetDetectDescription(const FDamageDetectDescriptionStruct& NewDetectDescription)
	{
		this->DamageDetectDescriptionStruct = NewDetectDescription;
	}

	virtual UParticleSystem* GetDamageIncreaseVFX()
	{
		return DamageIncreaseVFX;
	}

	virtual void SetDamageIncreaseVFX(UParticleSystem* CreatureDamageIncreaseVFX)
	{
		DamageIncreaseVFX = CreatureDamageIncreaseVFX;
	}

	virtual FDamageDetectedEvent& OnHitDetected()
	{
		return HitDetectedEvent;
	}
};
