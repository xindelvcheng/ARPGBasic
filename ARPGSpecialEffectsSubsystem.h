// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Sound/SoundCue.h"
#include "particles/ParticleSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGSpecialEffectsSubsystem.generated.h"

enum class ESoundEffectCategory:uint8
{
    Positive,
    Negative,
    Click,
    Confirm,
    Cancel
};

/**
 * 
 */
UCLASS()
class UARPGSpecialEffectsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    template <typename T>
    void LoadEffectsAssets(TArray<T*>& Effects, TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs);

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> PositiveSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> NegativeSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> ClickSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> ConfirmSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> CancelSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<UParticleSystem*> PositiveVisualEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<UParticleSystem*> NegativeVisualEffects;

    static UARPGSpecialEffectsSubsystem* Get(UWorld* World)
    {
        if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>())
        {
            return World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>();
        }

        return nullptr;
    }
};
