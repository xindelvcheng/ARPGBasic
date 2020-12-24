// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGDamageSubsystem.h"
#include "Components/BoxComponent.h"
#include "ARPGDamageBoxComponent.generated.h"
/**用于伤害检测，该组件的形状即为检测范围
 * 注意该组件不会自动激活，须在需要进行伤害检测时调用Activate()启动伤害判定，使用Deactivate()调用伤害判定
 */
UCLASS(ClassGroup=(ARPGBasic),meta=(BlueprintSpawnableComponent))
class UARPGDamageBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	TSubclassOf<UDamageType> DamageType;

	//基础伤害加成系数
	UPROPERTY(BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	float DamageWeight = 1;

	//基础伤害加成偏置
	UPROPERTY(BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	float DamageBias = 0;

	//速度伤害加成系数
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	float VelocityDamageBonusWeight = 0.01;

	UPROPERTY()
	UDamageDetectRecord* DamageDetectRecord;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	bool bDrawDebug;

	FDamageDetectDescriptionStruct DetectDescription;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGDamageBoxComponent",meta=(AllowPrivateAccess))
	UParticleSystem* DamageIncreaseVFX;

	void SetDamageValue(float DamageWeightCoefficient);
	void RegisterToDamageDetect();
	void UnRegisterToDamageDetectIfRegistered();
	
protected:
	virtual void OnDamageIncrease(float DeltaDamageWeightCoefficient);
	virtual void OnDamageDecrease(float DeltaDamageWeightCoefficient);

public:
	UARPGDamageBoxComponent();

	virtual void BeginPlay() override;


	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	
	float BaseDamage;

	UFUNCTION(BlueprintCallable,Category="ARPGDamageBoxComponent")
	void SetDamageType(TSubclassOf<UDamageType> DefaultDamageType) { DamageType = DefaultDamageType; }

	/*和其他伤害类型的伤害盒子重叠时自己的反应（元素相生相克）*/
	void ElementInteract(UARPGDamageBoxComponent* OtherDamageBoxComponent);

	virtual void Activate(bool bReset) override;

	virtual void Deactivate() override;


	virtual FDamageDetectDescriptionStruct GetDetectDescription() const { return DetectDescription; }

	virtual void SetDetectDescription(const FDamageDetectDescriptionStruct& NewDetectDescription)
	{
		this->DetectDescription = NewDetectDescription;
	}

	virtual UParticleSystem* GetDamageIncreaseVfx()
	{
		return DamageIncreaseVFX;
	}

	virtual void SetDamageIncreaseVfx(UParticleSystem* CreatureDamageIncreaseVfx)
	{
		DamageIncreaseVFX = CreatureDamageIncreaseVfx;
	}
};
