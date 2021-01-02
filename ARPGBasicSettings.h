﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGGamingMenuWidget.h"
#include "ARPGStatusWidget.h"
#include "ARPGWidgetsLab.h"
#include "CollectableItemWidget.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Sound/SoundCue.h"

#include "ARPGBasicSettings.generated.h"

class AAimTargetActor;
/**
 * 
 */
UCLASS(Config=ARPGBasicSettings,defaultconfig)
class UARPGBasicSettings : public UDeveloperSettings
{
    GENERATED_BODY()

    virtual FName GetContainerName() const override { return TEXT("Project"); }
    virtual FName GetCategoryName() const override { return TEXT("ARPGBasicSettings"); }
    virtual FName GetSectionName() const override { return TEXT("ARPGBasicSettings"); };

public:

    UFUNCTION(BlueprintPure,DisplayName="ARPGBasicSettings")
    static UARPGBasicSettings* Get()
    {
        return GetMutableDefault<UARPGBasicSettings>();
    }
    
    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UUserWidget> LoadingWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGStatusWidget> StatusWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGNotifyWidget> NotifyWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UCollectableItemWidget> GameItemWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGPromptWidget> PromptWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGLockTargetWidget> LockTargetWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGEnemyHPBarWidget> EnemyHPBarWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGPageContentItemWidget> GameMenuContentItemWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG 3D GUI Config")
    TSoftClassPtr<AAimTargetActor> AimPromptActorClass;
    
    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource Config")
    TArray<TSoftObjectPtr<UTexture2D>> Icons;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource Config")
    TSoftObjectPtr<UTexture2D> DefaultGameplayIcon;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource Config")
    TSoftObjectPtr<UTexture2D> DefaultGameItemIcon;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource Config")
    TSoftObjectPtr<UTexture2D> DefaultAbilityIcon;
    
    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TMap<FName,TSoftObjectPtr<USoundCue>> SoundEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TMap<FName,TSoftObjectPtr<UParticleSystem>> ParticleEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultGameItemPickUpPromptVisualEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultImpactVisualEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<USoundCue> DefaultImpactSoundEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultCreatureDestroyVisualEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<USoundCue> DefaultCreatureDestroySoundEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Special Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultDamageIncreaseVFX;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Gameplay Config")
    TSoftObjectPtr<UDataTable> AbilityConfigDataTable;
};
