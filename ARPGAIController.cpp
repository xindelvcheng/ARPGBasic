// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAIController.h"

#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGLockTargetComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void AARPGAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledCharacter = Cast<AARPGCharacter>(InPawn);
    checkf(ControlledCharacter.Get(), TEXT("ARPGAIContoller only can possess AARPGCharacter"));

    MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
    if (!MainCharacter.IsValid() && UARPGGameInstanceSubsystem::Get(GetWorld()))
    {
        UARPGGameInstanceSubsystem::Get(GetWorld())->OnPlayerSetupEnd.AddLambda([&]()
        {
            MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
        });
    }
}

AAction_MoveToTarget::AAction_MoveToTarget()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAction_MoveToTarget::SetKeepDistance(float NewDistance)
{
    KeepDistance = NewDistance;
}

void AAction_MoveToTarget::BeginPlay()
{
    Super::BeginPlay();
    

    TimerDelegate = FTimerDelegate::CreateLambda([&]()
    {
       Target = nullptr;
       GetWorldTimerManager().ClearTimer(TimerHandle);
       FinishAction();
    });
}

void AAction_MoveToTarget::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    if (!Target)
    {
        return;
    }
    const FVector Vector = Target->GetActorLocation() - OwnerCharacter->GetActorLocation();
    if (Vector.Size() <= KeepDistance)
    {
        FinishAction();
        return;
    }
    
    OwnerCharacter->AddMovementInput(Vector);


}

void AAction_MoveToTarget::OnActionActivate()
{
    Super::OnActionActivate();

    if (!GetWorldTimerManager().IsTimerActive(TimerHandle))
    {
        GetWorldTimerManager().SetTimer(TimerHandle,TimerDelegate,0.5,false);
    }
}

void AARPGSimpleAIController::BeginPlay()
{
    Super::BeginPlay();

    Action_MoveToTarget = GetWorld()->SpawnActor<AAction_MoveToTarget>();
    Action_MoveToTarget->InitWithOwningCharacter(ControlledCharacter.Get());
}

void AARPGSimpleAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
