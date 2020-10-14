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
    const FVector Vector = Target->GetActorLocation() - OwningCharacter->GetActorLocation();
    if (Vector.Size() <= KeepDistance)
    {
        FinishAction();
        return;
    }
    
    OwningCharacter->AddMovementInput(Vector);


}

void AAction_MoveToTarget::ActivateAction(AARPGCharacter* NewTarget)
{
    Super::ActivateAction(Target);
    
    Target = NewTarget;

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
    if (!MainCharacter.IsValid() || MainCharacter->GetIsRigid() || MainCharacter->GetIsActing())
    {
        return;
    }

    ControlledCharacter->SetActorRotation((MainCharacter->GetActorLocation()-ControlledCharacter->GetActorLocation()).ToOrientationRotator());
    
    const FVector Vector = MainCharacter->GetActorLocation() - ControlledCharacter->GetActorLocation();
    const float Distance = FVector::Distance(MainCharacter->GetActorLocation(),
                                             ControlledCharacter->GetActorLocation());

#if WITH_EDITOR
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::SanitizeFloat(Distance));
    }
#endif
    if (Distance > Threshold1)
    {
        Action_MoveToTarget->ActivateAction(MainCharacter.Get());
    }
    else if (Distance > Threshold2)
    {
        if (ControlledCharacter->GetCharacterCombatComponent()->RemoteAttackActions.Num() > 0)
        {
            ControlledCharacter->TryToRemoteAttack(
                FMath::RandRange(0, ControlledCharacter->GetCharacterCombatComponent()->RemoteAttackActions.Num() - 1));
        }else
        {
            Action_MoveToTarget->ActivateAction(MainCharacter.Get());
        }
    }
    else if (Distance > Threshold3)
    {
        if (ControlledCharacter->GetCharacterCombatComponent()->AbilityActions.Num() > 0)
        {
            if (FMath::FRand()>0.5)
            {
                ControlledCharacter->TryToUseAbility(
                FMath::RandRange(0, ControlledCharacter->GetCharacterCombatComponent()->AbilityActions.Num() - 1));
            }else
            {
                Action_MoveToTarget->ActivateAction(MainCharacter.Get());
            }
            
        }else
        {
            Action_MoveToTarget->ActivateAction(MainCharacter.Get());
        }
    }
    else if(Distance > Threshold4)
    {
        if (ControlledCharacter->GetCharacterCombatComponent()->AbilityActions.Num() > 0)
        {
            if (FMath::FRand()>0.5)
            {
                ControlledCharacter->TryToUseAbility(
                FMath::RandRange(0, ControlledCharacter->GetCharacterCombatComponent()->AbilityActions.Num() - 1));
            }else
            {
                ControlledCharacter->TryToMeleeAttack();
            }
        }else
        {
            ControlledCharacter->TryToMeleeAttack();
        }
        
    }else
    {
        ControlledCharacter->AddMovementInput(-Vector);
    }
}
