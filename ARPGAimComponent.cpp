// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAimComponent.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGMainCharacter.h"
#include "ARPGStaticFunctions.h"
#include "GameFramework/SpringArmComponent.h"

UARPGAimComponent::UARPGAimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;

	if (!AimPromptActorClass)
	{
		if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
		{
			AimPromptActorClass = BasicSettings->AimPromptActorClass.LoadSynchronous();
		}
	}
}

void UARPGAimComponent::BeginPlay()
{
	Super::BeginPlay();

	AimTargetActor = UARPGStaticFunctions::SpawnActor<AAimTargetActor>(
		AimPromptActorClass, FTransform{}, GetOwnerCharacter());

	bAimTargetResultIsValid = false;
	Deactivate();
}

void UARPGAimComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	AimTargetActor->SetActorVisibility(true);
	SetComponentTickEnabled(true);

	if (AARPGMainCharacter* Character = Cast<AARPGMainCharacter>(GetOwner()))
	{
		/*瞄准时调整视角*/
		Character->GetCameraBoom()->TargetArmLength = 0;
		Character->GetCameraBoom()->SetRelativeLocation(
			Character->GetMesh()->GetSocketTransform(TEXT("head"), ERelativeTransformSpace::RTS_Actor).GetLocation()+FVector{0,0,50});
	}
}

void UARPGAimComponent::Deactivate()
{
	Super::Deactivate();

	AimTargetActor->SetActorVisibility(false);
	SetComponentTickEnabled(false);

	if (AARPGMainCharacter* Character = Cast<AARPGMainCharacter>(GetOwner()))
	{
		/*取消瞄准时还原视角*/
		Character->GetCameraBoom()->TargetArmLength = 300;
		Character->GetCameraBoom()->SetRelativeLocation({0, 0, 0});
	}


	GetWorld()->GetTimerManager().SetTimer(ResetAimTargetTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		AimTargetActor->SetActorLocation(FVector{0, 0, 0});
		bAimTargetResultIsValid = true;
	}), AimTargetResetDuration, false);
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
		AimTargetActor->SetActorLocation(GetOwner()->GetActorLocation() + ViewInfo.Rotation.Vector() * AimRadius);
		/*如果角度不对就调整AAimTargetActor的本地朝向*/
		AimTargetActor->SetActorRotation(ViewInfo.Rotation);

		bAimTargetResultIsValid = true;
	}
}
