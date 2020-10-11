// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"

#include "ARPGNonPlayerCharacter.generated.h"

UCLASS(Blueprintable)
class AARPGAbility : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void Active(AARPGCharacter* User);
};


UCLASS()
class AARPGNonPlayerCharacter : public AARPGCharacter
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TArray<AARPGAbility*> RemoteAttackAbilities;

    UPROPERTY()
    TArray<AARPGAbility*> UltimateAbilities;


    bool IsRigid;


    int NormalAttackIndex = 0;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> NormalAttackMontages;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
    TArray<TSubclassOf<AARPGAbility>> RemoteAttackClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
    TArray<TSubclassOf<AARPGAbility>> UltimateAbilityClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
    UAnimMontage* AnimMontage_Rigid;

    UPROPERTY()
    UAnimInstance* AnimInstance;

    UFUNCTION()
    void OnMontageBegin(UAnimMontage* Montage);

    UFUNCTION()
    void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

    UFUNCTION()
    void OnMontageStop(UAnimMontage* Montage, bool bInterrupted);
public:
    // Sets default values for this character's properties
    AARPGNonPlayerCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    virtual void TryToNormalAttack();

    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    virtual void TryToRangeAttack();

    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    virtual void TryToUseAbility(int Index);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEvent);

    UPROPERTY(BlueprintCallable,BlueprintAssignable,Category="ARPGNonPlayerCharacter")
    FAttackEvent OnAttackOrAbilityComplete;


    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    void CauseRigid(float Duration);


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRigidEvent, float, Duration);

    UPROPERTY(BlueprintAssignable,Category="ARPGNonPlayerCharacter")
    FRigidEvent OnRigid;
    FRigidEvent OnResumeFromRigid;

    FTimerHandle RigidTimerHandle;
    FTimerDelegate RigidTimerDelegate;

    FTimerHandle PreparatoryTimerHandle;
    FTimerDelegate PreparatoryTimerDelegate;

    FTimerHandle MovingTimerHandle;
    FTimerDelegate MovingTimerDelegate;
    bool IsMoving;

#if WITH_EDITOR
    bool bDebug;

    UFUNCTION(CallInEditor,Category="ARPG NPC AI")
    void Debug() { bDebug = bDebug ? false : true; }
# endif

    
    void ResetMovingTimer()
    {
        IsMoving = true;
        GetWorldTimerManager().ClearTimer(MovingTimerHandle);
        GetWorldTimerManager().SetTimer(MovingTimerHandle, MovingTimerDelegate, 0.5, false);
    };
};

UCLASS()
class AARPGBossAI : public AARPGNonPlayerCharacter
{
    GENERATED_BODY()
};
