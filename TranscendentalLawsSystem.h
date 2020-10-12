// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "TranscendentalLawsSystem.generated.h"

class UARPGCharacterCombatComponent;
class AARPGCharacter;
class AARPGAction;
UENUM()
enum class ETranscendentalLawsAttainment:uint8
{
    Low,
    Middle,
    High,
    Perfect
};

UCLASS(Blueprintable,BlueprintType)
class ATranscendentalLawsSystem : public AActor
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<AARPGAction*> MeleeAttackCollectionActions;
    UPROPERTY()
    TArray<AARPGAction*> RemoteAttackActions;
    UPROPERTY()
    TArray<AARPGAction*> AbilityActions;
    UPROPERTY()
    TArray<AARPGAction*> BuffActions;

protected:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="TranscendentalLawsComponent")
    FName TranscendentalLawsName;

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="TranscendentalLawsComponent")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="TranscendentalLawsComponent")
    FName Category;

    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="TranscendentalLawsComponent")
    ETranscendentalLawsAttainment Attainment = ETranscendentalLawsAttainment::Low;

    AARPGCharacter* AttachedCharacter;
    UARPGCharacterCombatComponent* AttachedCharacterCombatComponent;


public:

    void SetAttachedCharacter(AARPGCharacter* NewAttachedCharacter);

    void SetCurrentState(ETranscendentalLawsAttainment NewState);

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> MeleeAttackClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> RemoteAttackClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> AbilityClasses;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
    TArray<TSubclassOf<AARPGAction>> BuffClasses;

    void Init(AARPGCharacter* NewAttachedCharacter, UARPGCharacterCombatComponent* NewAttachedCharacterCombatComponent);
    
    void Activate();
    void DeActivate();

    virtual void BeginPlay() override;
    
    void SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& Classes, TArray<AARPGAction*>& ActionActors);
};