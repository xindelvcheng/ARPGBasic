// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGLockTargetComponent.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGPlayerController.h"
#include "ARPGWidgetsLab.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UARPGLockTargetComponent::UARPGLockTargetComponent()
{
	
	AttachedCharacter = Cast<AARPGCharacter>(GetOwner());
	if (Cast<AARPGMainCharacter>(AttachedCharacter))
	{
		PrimaryComponentTick.bCanEverTick = true;
	}
	ActorsToIgnore.Emplace(AttachedCharacter);
	
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
	MainCharacterPlayerController = Cast<AARPGPlayerController>(AttachedCharacter->GetController());
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
	UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), AttachedCharacter->GetActorLocation(),
	                                              AttachedCharacter->GetActorLocation() + AttachedCharacter->
	                                              GetActorForwardVector() * DetectDistance,
	                                              DetectHalfSize,
	                                              AttachedCharacter->GetActorRotation(), ObjectTypes, false,
	                                              ActorsToIgnore,
	                                              bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	                                              OutHits, true);


	for (const FHitResult HitResult : OutHits)
	{
		const auto CandidateTarget = Cast<AARPGCharacter>(HitResult.GetActor());
		if (!LockedTargets.Contains(CandidateTarget) && CandidateTarget->GetCharacterLockTargetComponent())
		{
			LockingTarget = CandidateTarget;
			LockedTargets.Emplace(LockingTarget);
			LockingTarget->GetCharacterLockTargetComponent()->GetWidget()->SetVisibility(ESlateVisibility::Visible);
			SetComponentTickEnabled(true);
			return LockingTarget;
		}
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
