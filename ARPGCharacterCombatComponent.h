// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "ARPGCharacterCombatComponent.generated.h"

class AARPGCharacter;

UCLASS(Blueprintable)
class AARPGAction : public AActor
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadOnly,Category="ARPGAction")
    AARPGCharacter* OwningCharacter;

    UFUNCTION(BlueprintImplementableEvent,DisplayName="ActionActive")
    void BPFunc_Active(AARPGCharacter* Target = nullptr);
    UFUNCTION(BlueprintCallable,DisplayName="Action Finish")
    void FinishAction();

public:
    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter);

    virtual void ActivateAction(AARPGCharacter* Target = nullptr);
    virtual void Interrupt(AARPGCharacter* Causer);
    
    DECLARE_DELEGATE(FActionFinishDelegate);
    FActionFinishDelegate OnActionFinished;
};

UCLASS(Blueprintable)
class AARPGSimpleMontageAction : public AARPGAction
{
    GENERATED_BODY()

    UPROPERTY()
    UAnimInstance* AttachedCharacterAnimInstance;

protected:

    UFUNCTION()
    virtual void OnMontageBegin(UAnimMontage* Montage) {checkf(false,TEXT("此方法必须覆盖"))};
    UFUNCTION()
    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) {checkf(false,TEXT("此方法必须覆盖"))};
    UFUNCTION()
    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) {checkf(false,TEXT("此方法必须覆盖"))};

    virtual void Interrupt(AARPGCharacter* Causer) override;

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter) override;
};

UCLASS(Blueprintable)
class AARPGMeleeAttackAction : public AARPGSimpleMontageAction
{
    GENERATED_BODY()

    int MeleeAttackIndex;

    bool IsMeleeAttacking;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNormalAttacks",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> MeleeAttackMontages;

    virtual void ActivateAction(AARPGCharacter* Target) override;

    virtual void OnMontageBegin(UAnimMontage* Montage) override;
    
    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;

    virtual void Interrupt(AARPGCharacter* Causer) override;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UARPGCharacterCombatComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<AARPGAction*> MeleeAttackActions;
    UPROPERTY()
    TArray<AARPGAction*> RemoteAttackActions;
    UPROPERTY()
    TArray<AARPGAction*> AbilityActions;
    UPROPERTY()
    TArray<AARPGAction*> BuffActions;

    UPROPERTY()
    AARPGAction* CurrentActiveAction;

    UFUNCTION()
    void BindToOnActionFinished();

public:
    // Sets default values for this component's properties
    UARPGCharacterCombatComponent();

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> MeleeAttackClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> RemoteAttackClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> AbilityClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> BuffClasses; 
    
protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    AARPGCharacter* AttachedCharacter;

    bool IsRigid;

    bool IsActing;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent",meta=(AllowPrivateAccess=true))
    bool AllowInterruptBackswing;



    void SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& Classes,TArray<AARPGAction*>& ActionActors);
    
public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatEvent);

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionStart;

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionEnd;

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToMeleeAttack(int NormalAttackCollectionIndex);

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToRemoteAttack(int RemoteAttackIndex);

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToUseAbility(int AbilityIndex);

    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    void CauseRigid(float Duration, AARPGCharacter* Causer);


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRigidEvent, float, Duration);

    UPROPERTY(BlueprintAssignable,Category="ARPGNonPlayerCharacter")
    FRigidEvent OnRigid;
    FRigidEvent OnResumeFromRigid;

    FTimerHandle RigidTimerHandle;
    FTimerDelegate RigidTimerDelegate;

    bool IsMoving;
};
