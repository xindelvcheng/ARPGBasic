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

    if (GetMesh())
    {
        AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->OnMontageStarted.AddDynamic(this, &AARPGNonPlayerCharacter::OnMontageBegin);
            AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AARPGNonPlayerCharacter::OnMontageNotify);
            AnimInstance->OnMontageEnded.AddDynamic(this, &AARPGNonPlayerCharacter::OnMontageStop);
        }
    }

    RigidTimerDelegate.BindLambda([&]()
    {
        IsRigid = false;
        OnResumeFromRigid.Broadcast(GetWorldTimerManager().GetTimerElapsed(RigidTimerHandle));
    });

    MovingTimerDelegate = FTimerDelegate::CreateLambda([&]()
    {
        IsMoving = false;
    });
    PreparatoryTimerDelegate = FTimerDelegate::CreateLambda([&]()
    {
        AnimInstance->Montage_SetPlayRate(GetCurrentMontage(), 1);
    });


    for (auto RemoteAttackClass : RemoteAttackClasses)
    {
        AARPGAbility* Ability = Cast<AARPGAbility>(GetWorld()->SpawnActor(RemoteAttackClass));
        RemoteAttackAbilities.Add(Ability);
    }

    for (auto UltimateAbilityClass : UltimateAbilityClasses)
    {
        AARPGAbility* Ability = Cast<AARPGAbility>(GetWorld()->SpawnActor(UltimateAbilityClass));
        UltimateAbilities.Add(Ability);
    }
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


    if (IsRigid || !MainCharacter)
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


    if (Distance >= 3000)
    {
        ResetMovingTimer();
    }
    else if (Distance >= 1500)
    {
        if (FMath::FRand() > 0.2)
        {
            ResetMovingTimer();
        }
        else
        {
            TryToRangeAttack();
        }
    }
    else if (Distance >= 500)
    {
        if (FMath::FRand() > 0.7)
        {
            ResetMovingTimer();
        }
        else if(FMath::FRand() > 0.3)
        {
            TryToRangeAttack();
        }else
        {
            TryToUseAbility(0);
        }
    }
    else
    {
        if (FMath::FRand() > 0.2)
        {
            TryToNormalAttack();
        }
        else
        {
            TryToUseAbility(1);
        }
    }
}

void AARPGNonPlayerCharacter::TryToNormalAttack()
{
    if (!IsRigid)
    {
        PlayAnimMontage(NormalAttackMontages[NormalAttackIndex]);

        IsRigid = true;
        AnimInstance->Montage_SetPlayRate(GetCurrentMontage(), 0.3);
        GetWorldTimerManager().ClearTimer(PreparatoryTimerHandle);
        GetWorldTimerManager().SetTimer(PreparatoryTimerHandle, PreparatoryTimerDelegate, 0.5, false);
    }
}

void AARPGNonPlayerCharacter::TryToRangeAttack()
{
    RemoteAttackAbilities[0]->Active(this);
}

void AARPGNonPlayerCharacter::TryToUseAbility(int Index = 0)
{
    UltimateAbilities[Index]->Active(this);
}

void AARPGNonPlayerCharacter::CauseRigid(float Duration)
{
    if (!GetWorldTimerManager().TimerExists(RigidTimerHandle))
    {
        GetWorldTimerManager().SetTimer(RigidTimerHandle, RigidTimerDelegate, Duration, false);
        OnRigid.Broadcast(Duration);
    }
    else
    {
        float TimerRemaining = GetWorldTimerManager().GetTimerRemaining(RigidTimerHandle);
        TimerRemaining = Duration > TimerRemaining ? Duration : TimerRemaining;
        GetWorldTimerManager().ClearTimer(RigidTimerHandle);
        GetWorldTimerManager().SetTimer(RigidTimerHandle, RigidTimerDelegate, TimerRemaining
                                        , false);
        OnRigid.Broadcast(TimerRemaining);
    }
    IsRigid = true;
}

void AARPGNonPlayerCharacter::OnMontageBegin(UAnimMontage* Montage)
{
    IsRigid = true;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
}

void AARPGNonPlayerCharacter::OnMontageNotify(FName NotifyName,
                                              const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    IsRigid = false;
    NormalAttackIndex = (NormalAttackIndex + 1) % NormalAttackMontages.Num();
}

void AARPGNonPlayerCharacter::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted && Montage != AnimMontage_Rigid)
    {
        NormalAttackIndex = 0;
        IsRigid = false;
    }

    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}
