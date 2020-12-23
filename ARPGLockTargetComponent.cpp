// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGLockTargetComponent.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGPlayerController.h"
#include "ARPGWidgetsLab.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UARPGLockTargetComponent::UARPGLockTargetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
	if (UARPGBasicSettings::Get())
	{
		WidgetClass = UARPGBasicSettings::Get()->LockTargetWidgetClass.LoadSynchronous();
	}
	SetWidgetSpace(EWidgetSpace::Screen);
}


// Called when the game starts
void UARPGLockTargetComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	AttachedCharacter = Cast<AARPGCharacter>(GetOwner());
	ActorsToIgnore.Emplace(AttachedCharacter);
	if (Cast<AARPGMainCharacter>(AttachedCharacter))
	{
		MainCharacterPlayerController = Cast<AARPGPlayerController>(AttachedCharacter->GetController());
	}
}


// Called every frame
void UARPGLockTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (LockingTarget)
	{
		AttachedCharacter->SetActorRotation(
			(LockingTarget->GetActorLocation() - AttachedCharacter->GetActorLocation()).ToOrientationRotator());
	}
}

AARPGCharacter* UARPGLockTargetComponent::ToggleLockTarget()
{
	if (DetectLockTarget())
	{
		LockedTargets.Emplace(LockingTarget);
		LockingTarget->GetCharacterLockTargetComponent()->GetWidget()->SetVisibility(ESlateVisibility::Visible);
		SetComponentTickEnabled(true);
		return LockingTarget;
	}
	

	//未找到下一个满足条件的可锁定对象，视为玩家想解除当前锁定
	if (LockingTarget && LockingTarget->GetCharacterLockTargetComponent())
	{
		LockingTarget->GetCharacterLockTargetComponent()->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
		LockingTarget = nullptr;
		SetComponentTickEnabled(false);
	}
	LockedTargets.Empty();
	return nullptr;
}

AARPGCharacter* UARPGLockTargetComponent::DetectLockTarget()
{
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), AttachedCharacter->GetActorLocation(),
                                                  AttachedCharacter->GetActorLocation()+ AttachedCharacter->GetActorForwardVector()* DetectDistance,
                                                  DetectHalfSize,
                                                  AttachedCharacter->GetActorRotation()
                                                  , ETraceTypeQuery::TraceTypeQuery2, false,
                                                  ActorsToIgnore,
                                                  bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                                  OutHits, true);


	for (const FHitResult HitResult : OutHits)
	{
		const auto CandidateTarget = Cast<AARPGCharacter>(HitResult.GetActor());
		if (!LockedTargets.Contains(CandidateTarget) && CandidateTarget && CandidateTarget!=AttachedCharacter && CandidateTarget->GetCharacterLockTargetComponent())
		{
			LockingTarget = CandidateTarget;
			return LockingTarget;
		}
	}
	return nullptr;
}
