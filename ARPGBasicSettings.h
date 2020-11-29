// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGGamingMenuWidget.h"
#include "ARPGStatusWidget.h"
#include "ARPGWidgetsLab.h"
#include "GameItemWidget.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Sound/SoundCue.h"

#include "ARPGBasicSettings.generated.h"

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
    TSoftClassPtr<UARPGStatusWidget> LoadingWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGStatusWidget> StatusWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGNotifyWidget> NotifyWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UGameItemWidget> GameItemWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGPromptWidget> PromptWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGLockTargetWidget> LockTargetWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGEnemyHPBarWidget> EnemyHPBarWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
    TSoftClassPtr<UARPGGamingMenuContentItemWidget> GamingMenuContentItemWidgetClass;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource Config")
    TArray<TSoftObjectPtr<UTexture2D>> Icons;
    
    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Sound Effect Config")
    TArray<TSoftObjectPtr<USoundCue>> PositiveSoundEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Sound Effect Config")
    TArray<TSoftObjectPtr<USoundCue>> NegativeSoundEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Sound Effect Config")
    TArray<TSoftObjectPtr<USoundCue>> NeutralSoundEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Visual Effect Config")
    TArray<TSoftObjectPtr<UParticleSystem>> PositiveVisualEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Visual Effect Config")
    TArray<TSoftObjectPtr<UParticleSystem>> NegativeVisualEffects;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Visual Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultGameItemPickUpPromptVisualEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Visual Effect Config")
    TSoftObjectPtr<UParticleSystem> DefaultImpactVisualEffect;

    UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG Visual Effect Config")
    TSoftObjectPtr<USoundCue> DefaultImpactSoundEffect;
};
