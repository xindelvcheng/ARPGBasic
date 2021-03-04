// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
#include "ARPGDamageBoxComponent.generated.h"

class AARPGCharacter;

USTRUCT()
struct FDamageDetectDescriptionStruct
{
	GENERATED_BODY()

	bool bUseDamageCenterComponentCurrentBoundsAsDetectBound = true;
	/*DamageBoxHalfSizeInTrace在bUseDamageCenterComponentCurrentBoundsAsDetectBound = true（默认）时无效*/
	FVector DamageBoxHalfSizeInTrace = FVector(100, 100, 100);
	bool bCauseDamage = true;
	float DamageWeight = 1;
	float DamageBias = 10;
	float VelocityDamageBonusWeight = 0.01;
	TSubclassOf<UDamageType> DamageTypeClass;

	FDamageDetectDescriptionStruct() = default;

	/*使用UARPGDamageBoxComponent大小作为伤害检测范围*/
	FDamageDetectDescriptionStruct(bool bCauseDamage, float DamageWeight, float DamageBias,
	                               float VelocityDamageBonusWeight, const TSubclassOf<UDamageType>& DamageTypeClass)
		: bCauseDamage(bCauseDamage),
		  DamageWeight(DamageWeight),
		  DamageBias(DamageBias),
		  VelocityDamageBonusWeight(VelocityDamageBonusWeight),
		  DamageTypeClass(DamageTypeClass)
	{
	}

	/*手动指定伤害盒子的大小（而不使用UARPGDamageBoxComponent大小作为伤害检测范围）*/
	FDamageDetectDescriptionStruct(const FVector& DamageBoxHalfSizeInTrace, bool bCauseDamage, float DamageWeight,
	                               float DamageBias, float VelocityDamageBonusWeight,
	                               const TSubclassOf<UDamageType>& DamageTypeClass)
		: DamageBoxHalfSizeInTrace(DamageBoxHalfSizeInTrace),
		  bCauseDamage(bCauseDamage),
		  DamageWeight(DamageWeight),
		  DamageBias(DamageBias),
		  VelocityDamageBonusWeight(VelocityDamageBonusWeight),
		  DamageTypeClass(DamageTypeClass)
	{
	}
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

	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObjectsInContainer
	TSet<AActor*> ActorDamagedInSingleAttack;

	TArray<TEnumAsByte<EObjectTypeQuery>> DetectObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	bool bDrawDebug;

	FDamageDetectDescriptionStruct DamageDetectDescriptionStruct;

	UPROPERTY()
	UParticleSystem* DamageIncreaseVFX;

	DECLARE_MULTICAST_DELEGATE_OneParam(FDamageDetectedDelegate, FHitResult);
	FDamageDetectedDelegate HitDetectedDelegate;

	virtual void OnHitDetected(FHitResult HitResult);

	void CauseDamage(FHitResult HitResult);

protected:
	void SetDamageValue(float DamageWeightCoefficient);
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

	virtual FDamageDetectedDelegate& OnHitDetected()
	{
		return HitDetectedDelegate;
	}

	void SetDebugEnable(bool bShowDebug);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamageDetectedEvent, FHitResult, HitResult);
	UPROPERTY(BlueprintAssignable,Category="ARPGDamageBoxComponent")
	FDamageDetectedEvent DamageDetectedEvent;

	AARPGCharacter* GetOwnerCharacter() const;
};
