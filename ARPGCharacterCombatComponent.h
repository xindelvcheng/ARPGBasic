// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGAction.h"
#include "ARPGActorComponent.h"
#include "Components/ActorComponent.h"

#include "ARPGCharacterCombatComponent.generated.h"

class UCharacterConfigPrimaryDataAsset;
class AARPGAction;
class AARPGCastAction;
class AARPGCharacter;


UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGCharacterCombatComponent : public UARPGActorComponent
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<int, AARPGAction*> ExclusiveGroupActionsMap;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<FMeleeAttackActionDescriptionStruct> MeleeAttacks;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGCastAction>> AbilityClasses;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<FName> SpellNames;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGBuff>> BuffClasses;
public:
    // Sets default values for this component's properties
    UARPGCharacterCombatComponent();

    

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    bool IsRigid;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent",meta=(AllowPrivateAccess=true))
    bool AllowInterruptBackswing;


public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION()
    void BindToOnActionFinished(AARPGAction* Action);

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool GetIsRigid() const { return IsRigid; }

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool GetIsActing() const { return CurrentActiveAction ? true : false; }

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    AARPGAction* GetCurrentActiveAction() const { return CurrentActiveAction; }

    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<AARPGAction*> MeleeAttackCollectionActions;
    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    AARPGAction* CurrentMeleeAttackCollection;
    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<AARPGAction*> RemoteAttackActions;
    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<AARPGCastAction*> AbilityActions;
    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<AARPGBuff*> BuffActions;
    
    
    UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    AARPGAction* CurrentActiveAction;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatEvent);

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionStart;

    UPROPERTY(BlueprintAssignable,BlueprintCallable,Category="ARPGCharacterCombatComponent")
    FCombatEvent ActionEnd;

    UFUNCTION()
    virtual bool TryToMeleeAttack();

    UFUNCTION()
    virtual bool TryToUseAbility(int AbilityIndex);

    UFUNCTION()
    virtual bool CauseRigid(float Duration, AARPGCharacter* Causer);

    UFUNCTION()
    virtual bool ActivateBuff(int BuffIndex, float Duration, AARPGCharacter* Instigator = nullptr);


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRigidEvent, float, Duration);

    UPROPERTY(BlueprintAssignable,Category="ARPGCharacterCombatComponent")
    FRigidEvent OnRigid;
    FRigidEvent OnResumeFromRigid;

    FTimerHandle RigidTimerHandle;
    FTimerDelegate RigidTimerDelegate;

    bool IsMoving;

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void ReInitCharacterActions(UCharacterConfigPrimaryDataAsset* CharacterConfigPrimaryDataAsset = nullptr);


    virtual const TArray<FName>& GetSpellNames() const
    {
        return SpellNames;
    }

    virtual void SetSpellNames(const TArray<FName>& NewSpellNames)
    {
        this->SpellNames = NewSpellNames;
        ReInitCharacterActions();
    }

    virtual void AppendSpellNames(FName NewSpellName)
    {
        this->SpellNames.Emplace(NewSpellName);
        ReInitCharacterActions();
    }
};
