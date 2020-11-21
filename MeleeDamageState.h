// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeDamageState.generated.h"

class AARPGCharacter;
class UCharacterStatusComponent;
/**
 * 
 */
UCLASS()
class UMeleeDamageState : public UAnimNotifyState
{
    GENERATED_BODY()

    TArray<FHitResult> HitResult;

    UPROPERTY()
    TArray<AActor*> HitActors;

    UPROPERTY()
    AController* EventInstigator;

    FVector LastLocation1;

    UPROPERTY()
    AARPGCharacter* Player;
    FVector CurrentLocation1;
    FRotator CurrentRotation1;

    UPROPERTY()
    UCharacterStatusComponent* StatusComponent;

    UPROPERTY()
    float BaseAttack;

protected:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Melee Damage Trace")
    bool CauseDamage = true;

public:

    UPROPERTY(EditAnywhere,Category="Custom")
    FName DamageCenterSocketName = "DamageCenter";

    UPROPERTY(EditAnywhere,Category="Custom")
    FVector DamageBoxHalfSizeInTrace = FVector(10, 10, 100);

    UPROPERTY(EditAnywhere,Category="Custom")
    USkeletalMeshComponent* Weapon;


    //招式伤害加成系数
    UPROPERTY(EditAnywhere,Category="Custom")
    float DamageWeight = 1;

    //招式伤害加成偏置
    UPROPERTY(EditAnywhere,Category="Custom")
    float DamageBias = 0;

    //速度伤害加成系数
    UPROPERTY(EditAnywhere,Category="Custom")
    float VelocityDamageBonusWeight = 0.01;

    UPROPERTY(EditAnywhere,Category="Custom")
    TSubclassOf<UDamageType> DamageTypeClass;

    UPROPERTY(EditAnywhere,Category="Custom")
    TArray<AActor*> ActorsToIgnore;

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             float TotalDuration) override;
    virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                            float FrameDeltaTime) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
