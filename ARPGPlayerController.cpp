// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGPlayerController.h"


#include "ARPGBasicSettings.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGStatusWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AARPGPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    OnPlayerControllerPosses.Broadcast(Cast<ACharacter>(InPawn));

    const auto MainCharacter = Cast<AARPGMainCharacter>(InPawn);
    if (MainCharacter && GetGameInstance() && GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
    {
        UARPGGameInstanceSubsystem* ARPGGameInstanceSubsystem = GetGameInstance()->GetSubsystem<
            UARPGGameInstanceSubsystem>();
        ARPGGameInstanceSubsystem->MainCharacter = MainCharacter;
        ARPGGameInstanceSubsystem->MainCharacter->MainPlayerController = this;
        ARPGGameInstanceSubsystem->MainCharacterController = this;
        
        if (ARPGGameInstanceSubsystem->CurrentStreamingLevelName != "MainMenu")
        {
            if (UARPGBasicSettings::Get())
            {
                const auto StatusWidgetClass = LoadClass<UARPGStatusWidget>(
                    this, *(UARPGBasicSettings::Get()->StatusWidgetClass.ToString()));
                if (StatusWidgetClass)
                {
                    ARPGGameInstanceSubsystem->StatusWidget = Cast<UARPGStatusWidget>(
                        CreateWidget(this, StatusWidgetClass));
                    ARPGGameInstanceSubsystem->StatusWidget->AddToViewport();
                }
                else
                {
                    UARPGGameInstanceSubsystem::PrintLogToScreen(
                        TEXT("错误，未指定StatusWidgetClass或StatusWidgetClass无效"), 15, FColor::Red);
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
