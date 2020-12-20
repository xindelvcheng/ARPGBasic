// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGDamageSubsystem.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Components/BoxComponent.h"

#include "MeleeDamageDetectAnimNotifyState.generated.h"

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
    TWeakObjectPtr<UDamageDetectRecord> DamageDetectRecord;


protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    bool CauseDamage = true;

public:

    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    FName DamageCenterSocketName = "DamageCenter";

    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    FVector DamageBoxHalfSizeInTrace = FVector(10, 10, 100);

    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    UARPGDamageBoxComponent* WeaponDamageBoxCollision;

    //招式伤害加成系数
    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    float DamageWeight = 1;

    //招式伤害加成偏置
    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    float DamageBias = 0;

    //速度伤害加成系数
    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    float VelocityDamageBonusWeight = 0.01;

    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    TSubclassOf<UDamageType> DamageTypeClass;

    UPROPERTY(EditAnywhere,Category="MeleeDamageDetectAnimNotifyState")
    TArray<AActor*> ActorsToIgnore;

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
