// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "ARPGCharacterCombatComponent.generated.h"

class AARPGAction;
class AARPGCharacter;


UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGCharacterCombatComponent : public UActorComponent
{
    GENERATED_BODY()
    

public:
    // Sets default values for this component's properties
    UARPGCharacterCombatComponent();

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> MeleeAttackCollectionClasses;

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



    void SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& ActionClasses,TArray<AARPGAction*>& ActionActors);
    
public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void BindToOnActionFinished();

    
    UPROPERTY()
    TArray<AARPGAction*> MeleeAttackCollectionActions;
    AARPGAction* CurrentMeleeAttackCollection;
    UPROPERTY()
    TArray<AARPGAction*> RemoteAttackActions;
    UPROPERTY()
    TArray<AARPGAction*> AbilityActions;
    UPROPERTY()
    TArray<AARPGAction*> BuffActions;
    UPROPERTY()
    AARPGAction* CurrentActiveAction;
    
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatEvent);

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionStart;

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionEnd;

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToMeleeAttack();

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
