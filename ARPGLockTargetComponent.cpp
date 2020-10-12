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
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARPGLockTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AttachedCharacter = Cast<AARPGCharacter>(GetOwner());
	verifyf(AttachedCharacter,TEXT("锁定组件只能用于AARPGCharacter"));

	ActorsToIgnore.Emplace(AttachedCharacter);
	MainCharacterPlayerController = Cast<AARPGPlayerController>(AttachedCharacter->GetController());
	if (MainCharacterPlayerController && UARPGBasicSettings::Get())
	{
		const auto LockTargetWidgetClass= LoadClass<UARPGLockTargetWidget>(nullptr,*UARPGBasicSettings::Get()->LockTargetWidgetClass.ToString());
		LockTargetWidget = Cast<UARPGLockTargetWidget>(CreateWidget(MainCharacterPlayerController,LockTargetWidgetClass));
		if (LockTargetWidget)
		{
			LockTargetWidget->SetVisibility(ESlateVisibility::Hidden);
			LockTargetWidget->AddToViewport();
		}
	}
}


// Called every frame
void UARPGLockTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (LockingTarget)
	{
		AttachedCharacter->SetActorRotation((LockingTarget->GetActorLocation()-AttachedCharacter->GetActorLocation()).ToOrientationRotator());
		if (LockTargetWidget)
		{
			if(MainCharacterPlayerController->ProjectWorldLocationToScreen(LockingTarget->GetActorLocation(),LockIconScreenPosition))
			LockTargetWidget->SetLockIconScreenPosition(LockIconScreenPosition);
        }
	}

	
}

AARPGCharacter* UARPGLockTargetComponent::ToggleLockTarget()
{
	UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),AttachedCharacter->GetActorLocation(),
		AttachedCharacter->GetActorLocation()+AttachedCharacter->GetActorForwardVector()*500,
		HalfSize,
		AttachedCharacter->GetActorRotation(),ObjectTypes,false,ActorsToIgnore,EDrawDebugTrace::ForDuration,OutHits,true);

	for (const FHitResult HitResult : OutHits)
	{
		const auto CandidateTarget =  Cast<AARPGCharacter>(HitResult.GetActor());
		if (!LockedTargets.Contains(LockingTarget))
		{
			LockingTarget = CandidateTarget;
			LockedTargets.Emplace(LockingTarget);
			LockTargetWidget->SetVisibility(ESlateVisibility::Visible);
			return LockingTarget;
		}
	}

	//连按解除锁定
	LockingTarget = nullptr;
	LockedTargets.Empty();
	LockTargetWidget->SetVisibility(ESlateVisibility::Hidden);
	return nullptr;
}

