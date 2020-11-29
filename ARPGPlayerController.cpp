// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGPlayerController.h"



#include "ARPGArchiveSubsystem.h"
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


    const auto MainCharacter = Cast<AARPGMainCharacter>(InPawn);
    if (MainCharacter)
    {
        UARPGGameInstanceSubsystem* ARPGGameInstanceSubsystem = UARPGGameInstanceSubsystem::Get(GetWorld());
        MainCharacter->MainPlayerController = this;
        UARPGStatusWidget* MainCharacterStatusWidget =
            UARPGGameInstanceSubsystem::GetMainCharacterStatusWidget(GetWorld());
        if (MainCharacterStatusWidget)
        {
            MainCharacterStatusWidget->BindToMainCharacter(MainCharacter);
        }
        else if (UARPGArchiveSubsystem::Get(GetWorld())||UARPGArchiveSubsystem::Get(GetWorld())->CurrentStreamingLevelName != "MainMenu")
        {
            MainCharacterStatusWidget = Cast<UARPGStatusWidget>(
                CreateWidget(this, StatusWidgetClass));
            if (StatusWidgetClass && MainCharacterStatusWidget)
            {
                MainCharacterStatusWidget->AddToViewport();
                MainCharacterStatusWidget->BindToMainCharacter(MainCharacter);
            }
        }
        ARPGGameInstanceSubsystem->SetupPlayer(MainCharacter,this,MainCharacterStatusWidget);
    }
#if WITH_EDITOR
    else
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，指定主角未继承自MainCharacter"), 15, FColor::Red);
    }
#endif
}
