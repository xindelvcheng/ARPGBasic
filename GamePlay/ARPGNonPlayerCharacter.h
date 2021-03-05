// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "ARPGPlayerController.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Sound/SoundCue.h"

#include "ARPGNonPlayerCharacter.generated.h"

class UWidgetComponent;
class UARPGEnemyHPBarWidget;

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

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter")
    UWidgetComponent* HPBarWidgetComponent;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter")
    FVector HPBarOffsetVector{0,0,100};

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter")
    UAIPerceptionComponent* AIPerceptionComponent;

    UFUNCTION()
    void BindToSensePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    void ShowHPBar();

    void HideHPBar();

    UFUNCTION()
    void OnNPCDeath();

    FTimerHandle ForgetTargetTimerHandle;
};



UCLASS()
class AARPGBossAI : public AARPGNonPlayerCharacter
{
    GENERATED_BODY()
};
