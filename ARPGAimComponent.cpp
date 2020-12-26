// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAimComponent.h"


#include "ARPGCharacter.h"
#include "ARPGConfigSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "particles/ParticleSystemComponent.h"

UARPGAimComponent::UARPGAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
}

void UARPGAimComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UARPGConfigSubsystem* ConfigSubsystem = UARPGConfigSubsystem::Get(GetWorld()))
	{
		AimTargetActor = UARPGGameInstanceSubsystem::SpawnActor<AAimTargetActor>(
			ConfigSubsystem->AimPromptActorClass, GetComponentTransform(), GetOwnerCharacter());
	}

	Deactivate();
}

void UARPGAimComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	AimTargetActor->SetActorVisibility(true);
	SetComponentTickEnabled(true);
	
}

void UARPGAimComponent::Deactivate()
{
	Super::Deactivate();

	AimTargetActor->SetActorVisibility(false);
	SetComponentTickEnabled(false);
}

void AAimTargetActor::BeginPlay()
{
	Super::BeginPlay();
}

AAimTargetActor::AAimTargetActor()
{
	SetActorEnableCollision(false);
}
void UARPGAimComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerCharacter())
	{
		FMinimalViewInfo ViewInfo;
		GetOwnerCharacter()->CalcCamera(0, ViewInfo);
		const TArray<AActor*> IgnoreActors = {GetOwnerCharacter()};
		FHitResult HitResult;
		UKismetSystemLibrary::LineTraceSingle(this, ViewInfo.Location,
		                                      ViewInfo.Location + ViewInfo.Rotation.Vector() * 3000,
		                                      ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors,
		                                      EDrawDebugTrace::None, HitResult, true);
		AimTargetActor->SetActorLocation(HitResult.Location);
	}
}
