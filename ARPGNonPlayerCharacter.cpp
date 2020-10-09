// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGNonPlayerCharacter.h"


// Sets default values
AARPGNonPlayerCharacter::AARPGNonPlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
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
    
}

// Called every frame
void AARPGNonPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AARPGNonPlayerCharacter::TryToNormalAttack()
{
    if (!IsRigid)
    {
        PlayAnimMontage(NormalAttackMontages[NormalAttackIndex]);
        
        IsRigid = true;
    }
}

void AARPGNonPlayerCharacter::TryToRangeAttack()
{
    TryToNormalAttack();
}

void AARPGNonPlayerCharacter::TryToUseAbility()
{
    TryToNormalAttack();
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
        GetWorldTimerManager().SetTimer(RigidTimerHandle, RigidTimerDelegate,TimerRemaining
                                        , false);
        OnRigid.Broadcast(TimerRemaining);
    }
    IsRigid = true;
}

void AARPGNonPlayerCharacter::OnMontageBegin(UAnimMontage* Montage)
{
}

void AARPGNonPlayerCharacter::OnMontageNotify(FName NotifyName,
                                              const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    IsRigid = false;
    NormalAttackIndex = (NormalAttackIndex + 1) % NormalAttackMontages.Num();
}

void AARPGNonPlayerCharacter::OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted && Montage!= AnimMontage_Rigid)
    {
        NormalAttackIndex = 0;
        IsRigid = false;
    }
}
