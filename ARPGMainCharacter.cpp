// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGMainCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterStatusComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AARPGMainCharacter::AARPGMainCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    GameItemsManagerComponent = CreateDefaultSubobject<UARPGGameItemsManagerComponent>("GameItemsManagerComponent");
}

// Called when the game starts or when spawned
void AARPGMainCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (MainPlayerController)
    {
        MainPlayerController->SetInputMode(FInputModeGameOnly{});
        MainPlayerController->bShowMouseCursor = false;
    }
}

// Called every frame
void AARPGMainCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AARPGMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
