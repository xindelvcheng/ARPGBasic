// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/BoxComponent.h"

#include "MeleeDamageDetectAnimNotifyState.generated.h"

class UARPGDamageBoxComponent;
class AARPGCharacter;
class UCharacterStatusComponent;
/**
 * 
 */
UCLASS()
class UMeleeDamageDetectAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	TArray<FHitResult> HitResults;
	UPROPERTY()
	AARPGCharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly,Category="MeleeDamageDetectAnimNotifyState",meta=(AllowPrivateAccess="true"))
	TArray<UARPGDamageBoxComponent*> DamageBoxes;

	UPROPERTY(BlueprintReadOnly,Category="MeleeDamageDetectAnimNotifyState",meta=(AllowPrivateAccess="true"))
	TArray<AActor*> ActorsToIgnore;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
	bool bDrawDebug = false;

	

public:

	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
	FName DamageCenterSocketName = "DamageCenter";

	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
	bool bManualSetDamageDetectBoxSize = false;

	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState",meta=(EditCondition="bManualSetDamageDetectBoxSize",EditConditionHides))
	FVector DamageBoxHalfSizeInTrace = FVector{10, 10, 100};

	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
	TArray<FName> DamageBoxNames = {"WeaponDamageBox"};

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
	bool bCauseDamage = true;

	//招式伤害加成系数
	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState",meta=(EditCondition="bCauseDamage",EditConditionHides))
	float DamageWeight = 1;

	//招式伤害加成偏置
	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState",meta=(EditCondition="bCauseDamage",EditConditionHides))
	float DamageBias = 0;

	//速度伤害加成系数
	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState",meta=(EditCondition="bCauseDamage",EditConditionHides))
	float VelocityDamageBonusWeight = 0.01;

	UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState",meta=(EditCondition="bCauseDamage",EditConditionHides))
	TSubclassOf<UDamageType> DamageTypeClass;


protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                         float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	void FindDamageBoxesByNames();
};
