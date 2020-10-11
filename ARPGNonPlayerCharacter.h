// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"

#include "ARPGNonPlayerCharacter.generated.h"

UCLASS()
class AARPGNonPlayerCharacter : public AARPGCharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AARPGNonPlayerCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEvent);

    UPROPERTY(BlueprintCallable,BlueprintAssignable,Category="ARPGNonPlayerCharacter")
    FAttackEvent OnAttackOrAbilityComplete;


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
