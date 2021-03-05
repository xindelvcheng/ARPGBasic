// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGMainMenuWidget.h"

bool UARPGMainMenuWidget::Initialize()
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
	AttachedController = Cast<APlayerController>(GetOwningPlayer());
	if (AttachedController)
	{
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(AttachedController,this,EMouseLockMode::DoNotLock);
		AttachedController->bShowMouseCursor = true;
	}
       
            
	Button_StartNewGame->OnClicked.AddDynamic(this, &UARPGMainMenuWidget::OnClickButton_StartNewGame);
	Button_QuitGame->OnClicked.AddDynamic(this, &UARPGMainMenuWidget::OnClickButton_QuitGame);

	return true;
}

void UARPGMainMenuWidget::OnClickButton_StartNewGame()
{
	// UGameplayStatics::OpenLevel(GetWorld(),TEXT("ModernCityMap01"));
}

void UARPGMainMenuWidget::OnClickButton_QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),UGameplayStatics::GetPlayerController(GetWorld(),0),EQuitPreference::Quit,true);
}
