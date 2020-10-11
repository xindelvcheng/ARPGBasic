// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGNonPlayerCharacter.h"

#include "ARPGGameInstanceSubsystem.h"
#include "Tasks/AITask_MoveTo.h"
#include "AIModule/Classes/AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AARPGNonPlayerCharacter::AARPGNonPlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AARPGNonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AARPGNonPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    AARPGMainCharacter* MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
    const float Distance = FVector::Distance(MainCharacter->GetActorLocation(), this->GetActorLocation());

#if WITH_EDITOR
     if (GEngine && bDebug)
     {
         GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::SanitizeFloat(Distance));
     }
#endif


    if (!MainCharacter)
    {
        return;
    }

    GetMovementComponent()->AddInputVector(MainCharacter->GetActorLocation() - GetActorLocation());
    const FVector Direction = MainCharacter->GetActorLocation() - GetActorLocation();
    FRotator NewControlRotation = Direction.Rotation();

    NewControlRotation.Yaw = FRotator::ClampAxis(NewControlRotation.Yaw);
    SetActorRotation(NewControlRotation);


    if (IsMoving && Distance > 500)
    {
        GetMovementComponent()->AddInputVector(MainCharacter->GetActorLocation() - GetActorLocation());
        return;
    }
    else
    {
        IsMoving = false;
        GetWorldTimerManager().ClearTimer(MovingTimerHandle);
    }

}