// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGPlayerController.h"


#include "ARPGBasicSettings.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGStatusWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


AARPGPlayerController::AARPGPlayerController()
{
    if (UARPGBasicSettings::Get())
    {
        StatusWidgetClass = LoadClass<UARPGStatusWidget>(
                    this, *(UARPGBasicSettings::Get()->StatusWidgetClass.ToString()));
    }
}

void AARPGPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    OnPlayerControllerPosses.Broadcast(Cast<ACharacter>(InPawn));

    const auto MainCharacter = Cast<AARPGMainCharacter>(InPawn);
    if (MainCharacter && GetGameInstance() && GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
    {
        UARPGGameInstanceSubsystem* ARPGGameInstanceSubsystem = GetGameInstance()->GetSubsystem<
            UARPGGameInstanceSubsystem>();
        ARPGGameInstanceSubsystem->SetMainCharacter(MainCharacter);
        MainCharacter->MainPlayerController = this;
        ARPGGameInstanceSubsystem->SetMainCharacterController(this);

        if (ARPGGameInstanceSubsystem->CurrentStreamingLevelName != "MainMenu")
        {
            UARPGStatusWidget* MainCharacterStatusWidget = UARPGGameInstanceSubsystem::GetMainCharacterStatusWidget(GetWorld());
            if (MainCharacterStatusWidget)
            {
                MainCharacterStatusWidget->BindToMainCharacter(MainCharacter);
            }
            else 
            {
                MainCharacterStatusWidget = Cast<UARPGStatusWidget>(
                    CreateWidget(this, StatusWidgetClass));
                if (StatusWidgetClass && MainCharacterStatusWidget)
                {
                    ARPGGameInstanceSubsystem->SetMainCharacterStatusWidget(MainCharacterStatusWidget);
                    MainCharacterStatusWidget->AddToViewport();
                    MainCharacterStatusWidget->BindToMainCharacter(MainCharacter);
                }
            }
        }
    }
#if WITH_EDITOR
    else
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，指定主角未继承自MainCharacter"), 15, FColor::Red);
    }
#endif
}

