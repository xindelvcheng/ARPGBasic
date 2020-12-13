// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "APRGGameSaver.h"
#include "ARPGGameInstanceSubsystem.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"


#include "ARPGMainMenuWidget.generated.h"


UCLASS(Blueprintable)
class  UARPGLoadingScreenWidget : public UUserWidget
{
    GENERATED_BODY()
};


UCLASS(Blueprintable)
class  UARPGMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="ARPGMainMenuWidget")
    UButton* Button_StartNewGame;

    UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="ARPGMainMenuWidget")
    UButton* Button_ContinueGame;

    UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="ARPGMainMenuWidget")
    UButton* Button_LoadGame;

    UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="ARPGMainMenuWidget")
    UButton* Button_Config;

    UPROPERTY(VisibleAnywhere,meta=(BindWidget),Category="ARPGMainMenuWidget")
    UButton* Button_QuitGame;

    
    

protected:
    UPROPERTY(BlueprintReadOnly)
    APlayerController* AttachedController;
    
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UARPGLoadingScreenWidget> ARPGLoadingWidgetClass;


    virtual bool Initialize() override;

    UFUNCTION()
    void OnClickButton_StartNewGame();

    UFUNCTION()
    void OnClickButton_QuitGame();
};
