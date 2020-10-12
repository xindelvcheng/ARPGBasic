// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "GameFramework/Character.h"
#include "ARPGCharacter.generated.h"

UCLASS()
class AARPGCharacter : public ACharacter
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC",meta=(AllowPrivateAccess))
    FName CharacterName = "DefaultCharacter";



    
public:
    // Sets default values for this character's properties
    AARPGCharacter();


    FName GetCharacterName() const
    {
        return CharacterName;
    }

    void SetCharacterName(const FName& NewCharacterName)
    {
        this->CharacterName = NewCharacterName;
        CharacterStatusComponent->SetCharacterName(CharacterName);
    }

    
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
    UARPGCharacterCombatComponent* GetCharacterCombatComponent()const
    {
        return CharacterCombatComponent;
    }
    
    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToMeleeAttack()const
    {
        CharacterCombatComponent->TryToMeleeAttack();
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToRemoteAttack(const int RemoteAttackIndex)const
    {
        CharacterCombatComponent->TryToRemoteAttack(RemoteAttackIndex);
    };

    UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
    void TryToUseAbility(const int AbilityIndex) const
    {
        CharacterCombatComponent->TryToUseAbility(AbilityIndex);
    };

    UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    void CauseRigid(const float Duration, AARPGCharacter* Causer)const
    {
        CharacterCombatComponent->CauseRigid(Duration,Causer);
    };

};
