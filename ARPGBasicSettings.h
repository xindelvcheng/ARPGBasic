// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGStatusWidget.h"
#include "ARPGWidgetsLab.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "ARPGBasicSettings.generated.h"

/**
 * 
 */
UCLASS(Config=ARPGBasicSettings)
class  UARPGBasicSettings : public UDeveloperSettings
{
	GENERATED_BODY()
    
	virtual FName GetContainerName() const override{return TEXT("Project");}
	virtual FName GetCategoryName() const override{return TEXT("UARPGBasicSettings");}

public:
    
	UFUNCTION(BlueprintPure,DisplayName="ARPGBasicSettings")
    static UARPGBasicSettings* Get()
	{
		return GetMutableDefault<UARPGBasicSettings>();
	}
    
	UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
	TSoftClassPtr<UARPGStatusWidget> StatusWidgetClass;
	
	UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
	TSoftClassPtr<UARPGNotifyWidget> NotifyWidgetClass;

		
	UPROPERTY(Config,EditAnywhere,BlueprintReadWrite,Category="ARPG GUI Config")
	TSoftClassPtr<UARPGPromptWidget> PromptWidgetClass;
	
	UPROPERTY(Config=Game,EditAnywhere,BlueprintReadWrite,Category="Data Config")
	TSoftObjectPtr<UDataTable> CharactersConfig;

	UPROPERTY(Config=Game,EditAnywhere,BlueprintReadWrite,Category="Transcendental Laws Constructor Config")
	TSoftObjectPtr<UDataTable> LevelToRealmConfig;
};
