// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGUISubsystem.h"
#include "TranscendentalCombatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "functional"


#include "ARPGGamingMenuWidget.generated.h"


class UARPGGamingMenuPageWidget;
class UARPGGamingMenuContentItemWidget;
class UGamingMenuTitleItemWidgetConfig;

/**
 * 
 */
UCLASS()
class UARPGGameMenuWidget : public UARPGTabWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	TArray<TSubclassOf<UARPGPageWidget>> PageClasses;
public:
	UARPGGameMenuWidget();

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;
};


UCLASS()
class UARPGSpellManagerPageWidget : public UARPGPageWidget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(AllowPrivateAccess))
	TMap<UARPGButton*,AARPGCastAction*> ButtonSpellMap;

	virtual TArray<UARPGPageContentItemWidget*> SetupContent() override;
};

