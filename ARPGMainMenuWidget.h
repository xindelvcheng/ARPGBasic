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


    virtual bool Initialize() override
    {
        Super::Initialize();

        if (!Button_StartNewGame || !Button_ContinueGame || !Button_LoadGame || !Button_Config || !Button_QuitGame)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("蓝图类与C++类的按钮设置未对应"));
                return false;
            }
        }
        else if (!ARPGLoadingWidgetClass)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("未指定加载页面蓝图类"));
                return false;
            }
        }
        AttachedController = Cast<APlayerController>(GetOwningPlayer());
        if (AttachedController)
        {
            UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(AttachedController,this,EMouseLockMode::DoNotLock);
            AttachedController->bShowMouseCursor = true;
        }
       
            
        Button_StartNewGame->OnClicked.AddDynamic(this, &UARPGMainMenuWidget::OnClickButton_StartNewGame);
        Button_QuitGame->OnClicked.AddDynamic(this, &UARPGMainMenuWidget::OnClickButton_QuitGame);

        return true;
    };

    UFUNCTION(BlueprintImplementableEvent)
    void OnClickButton_StartNewGame();

    UFUNCTION()
    void OnClickButton_QuitGame()
    {
        UKismetSystemLibrary::QuitGame(GetWorld(),UGameplayStatics::GetPlayerController(GetWorld(),0),EQuitPreference::Quit,true);
    }
};
