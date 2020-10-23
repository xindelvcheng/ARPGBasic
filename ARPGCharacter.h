// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

class UCharacterConfigPrimaryDataAsset;

UENUM()
enum class EActionResult:uint8
{
    ActionSuccess,
    ActionFail
};

UCLASS()
class AARPGCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC",meta=(AllowPrivateAccess))
    UCharacterConfigPrimaryDataAsset* CharacterConfigPDataAsset;

public:
    // Sets default values for this character's properties
    AARPGCharacter();

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC",meta=(AllowPrivateAccess))
    FName CharacterName = "DefaultCharacter";
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
    class UCharacterStatusComponent* CharacterStatusComponent;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
    class UTranscendentalCombatComponent* CharacterCombatComponent;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
    class UARPGLockTargetComponent* CharacterLockTargetComponent;

public:

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


    UCharacterStatusComponent* GetCharacterStatusComponent() const
    {
        return CharacterStatusComponent;
    }

    UARPGLockTargetComponent* GetCharacterLockTargetComponent() const
    {
        return CharacterLockTargetComponent;
    }

    FText GetCharacterDisplayName() const;

    //   转发常用属性到CharacterStatusComponent

    UFUNCTION(BlueprintPure,Category="ARPGBASIC")
    int GetCharacterLevel() const
    {
        return CharacterStatusComponent->GetLevel();
    }

    UFUNCTION(BlueprintPure,Category="ARPGBASIC")
    int GetMaxHP() const
    {
        return CharacterStatusComponent->GetMaxHP();
    }


    UFUNCTION(BlueprintPure,Category="ARPGBASIC")
    int GetCurrentHP() const
    {
        return CharacterStatusComponent->GetCurrentHP();
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void SetCurrentHP(const int CurrentHP) const
    {
        CharacterStatusComponent->SetCurrentHP(CurrentHP);
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void UpdateCurrentHP(const int DeltaHP) const
    {
        CharacterStatusComponent->UpdateCharacterHP(DeltaHP);
    }

    UFUNCTION(BlueprintPure,Category="ARPGBASIC")
    int GetMaxSP() const
    {
        return CharacterStatusComponent->GetMaxSP();
    }

    UFUNCTION(BlueprintPure,Category="ARPGBASIC")
    int GetCurrentSP() const
    {
        return CharacterStatusComponent->GetCurrentSP();
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void SetCurrentSP(const int CurrentSP) const
    {
        CharacterStatusComponent->SetCurrentSP(CurrentSP);
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void UpdateCurrentSP(const int DeltaSP) const
    {
        CharacterStatusComponent->UpdateCharacterSP(DeltaSP);
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    int GetCoins() const
    {
        return CharacterStatusComponent->GetCoins();
    }

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void UpdateCoins(const int DeltaCoins) const
    {
        CharacterStatusComponent->UpdateCharacterCoins(DeltaCoins);
    }

    FCharacterPropertyChangedEvent& OnCharacterPropertyChanged() const
    {
        return CharacterStatusComponent->OnCharacterPropertyChanged;
    }

    //转发常用属性到ARPGCharacterCombatComponent
    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    UTranscendentalCombatComponent* GetCharacterCombatComponent() const
    {
        return CharacterCombatComponent;
    }

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool TryToMeleeAttack()
    {
        return CharacterCombatComponent->TryToMeleeAttack();
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool TryToRemoteAttack(const int RemoteAttackIndex)
    {
        return CharacterCombatComponent->TryToRemoteAttack(RemoteAttackIndex);
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool TryToUseAbility(const int AbilityIndex)
    {
        return (CharacterCombatComponent->TryToUseAbility(AbilityIndex));
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool CauseRigid(const float Duration, AARPGCharacter* Causer)
    {
        return CharacterCombatComponent->CauseRigid(Duration, Causer);
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool GetIsRigid() const { return CharacterCombatComponent->GetIsRigid(); }

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    bool GetIsActing() const { return CharacterCombatComponent->GetIsActing(); }

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    AARPGAction* GetCurrentActiveAction() const { return CharacterCombatComponent->GetCurrentActiveAction(); }
};
