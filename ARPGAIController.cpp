// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAIController.h"

#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"

void AARPGAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledCharacter = Cast<AARPGCharacter>(InPawn);
    checkf(ControlledCharacter.Get(), TEXT("ARPGAIContoller only can possess AARPGCharacter"));

    MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
    if(!MainCharacter.IsValid() && UARPGGameInstanceSubsystem::Get(GetWorld()))
    {
        UARPGGameInstanceSubsystem::Get(GetWorld())->OnPlayerSetupEnd.AddLambda([&]()
        {
            MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
        });
    }
}

void AARPGAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
