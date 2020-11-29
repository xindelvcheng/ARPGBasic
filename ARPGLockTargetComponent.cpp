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
	Widget->SetVisibility(ESlateVisibility::Hidden);
	AttachedCharacter = Cast<AARPGCharacter>(GetOwner());
	ActorsToIgnore.Emplace(AttachedCharacter);
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
	                                              GetActorForwardVector() * 500,
	                                              HalfSize,
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
			LockingTarget->GetCharacterLockTargetComponent()->Widget->SetVisibility(ESlateVisibility::Visible);
			return LockingTarget;
		}
	}

	//未找到满足条件的对象，视为玩家想解除当前锁定
	if (LockingTarget && LockingTarget->GetCharacterLockTargetComponent())
	{
		LockingTarget->GetCharacterLockTargetComponent()->Widget->SetVisibility(ESlateVisibility::Hidden);
		LockingTarget = nullptr;
	}
	LockedTargets.Empty();
	return nullptr;
}
