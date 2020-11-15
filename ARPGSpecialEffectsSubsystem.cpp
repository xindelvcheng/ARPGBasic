// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpecialEffectsSubsystem.h"

#include "ARPGBasicSettings.h"

template<typename T>
void UARPGSpecialEffectsSubsystem::LoadEffectsAssets(TArray<T*>& Effects,
                                              TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs)
{
    for (TSoftObjectPtr<T> Effect : EffectSoftObjectPtrs)
    {
        Effects.Emplace(Effect.LoadSynchronous());
    }
}

void UARPGSpecialEffectsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UARPGBasicSettings::Get())
    {
        const auto ARPGBasicSettings = UARPGBasicSettings::Get();
        LoadEffectsAssets(PositiveSoundEffects, ARPGBasicSettings->PositiveSoundEffects);
        LoadEffectsAssets(NegativeSoundEffects, ARPGBasicSettings->NegativeSoundEffects);
        LoadEffectsAssets(ClickSoundEffects, ARPGBasicSettings->ClickSoundEffects);
        LoadEffectsAssets(ConfirmSoundEffects, ARPGBasicSettings->ConfirmSoundEffects);
        LoadEffectsAssets(CancelSoundEffects, ARPGBasicSettings->CancelSoundEffects);
        LoadEffectsAssets(PositiveVisualEffects,ARPGBasicSettings->PositiveVisualEffects);
        LoadEffectsAssets(NegativeVisualEffects,ARPGBasicSettings->NegativeVisualEffects);
    }
}
