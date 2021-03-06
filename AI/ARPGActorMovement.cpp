﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActorMovement.h"

#include <complex>

#include "ARPGStaticFunctions.h"
#include "Kismet/KismetStringLibrary.h"


// Sets default values for this component's properties
UARPGActorMovementComponent::UARPGActorMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
	// ...
}

void UARPGActorMovementComponent::Move(FMoveFunction NewMoveFunction)
{
	Timer = 0;
	OriginTransform = GetOwner()->GetActorTransform();
	MoveFunction = NewMoveFunction;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UARPGActorMovementComponent::MoveForward()
{
	FMoveFunction NewMoveFunction;
	NewMoveFunction.BindDynamic(this, &UARPGActorMovementComponent::MoveForwardFunction);
	Move(NewMoveFunction);
}

void UARPGActorMovementComponent::Stop()
{
	MoveFunction = FMoveFunction{};
	PrimaryComponentTick.SetTickFunctionEnable(false);
}


// ReSharper disable once CppMemberFunctionMayBeConst
FTransform UARPGActorMovementComponent::MoveForwardFunction(float Time)
{
	FTransform Transform;

	if (AActor* OwnerActor = GetOwner())
	{
		// const FVector XYForwardVector = FVector{OwnerActor->GetActorForwardVector().X,OwnerActor->GetActorForwardVector().Y,0}.GetSafeNormal();
		Transform.SetLocation(OwnerActor->GetActorForwardVector().GetSafeNormal() * Time * 5);
	}

	return Transform;
}

// Called every frame
void UARPGActorMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (MoveFunction.IsBound())
	{
		Timer += DeltaTime;
		if (AActor* Actor = GetOwner())
		{
			Actor->SetActorTransform({
				OriginTransform.GetRotation() + MoveFunction.Execute(Timer).GetRotation(),
				OriginTransform.GetLocation() + MoveFunction.Execute(Timer).GetLocation().X * 100 * OriginTransform.
				Rotator().GetComponentForAxis(EAxis::X) + MoveFunction.Execute(Timer).GetLocation().Y * 100 *
				OriginTransform.
				Rotator().GetComponentForAxis(EAxis::Y) + MoveFunction.Execute(Timer).GetLocation().Z * 100 *
				OriginTransform.
				Rotator().GetComponentForAxis(EAxis::Z),
				OriginTransform.GetScale3D() * MoveFunction.Execute(Timer).GetScale3D()
			});
		};
	}
}

void UARPGActorTowardsActorMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetActor.IsValid())
	{
		GetOwner()->AddActorWorldOffset((TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()) * DeltaTime);
		SetOriginTransform(GetOwner()->GetActorTransform());
	}
}

UARPGActorTowardsActorMovementComponent::UARPGActorTowardsActorMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UARPGActorTowardsActorMovementComponent::MoveTowardsActor(AActor* Target, float NewAcceptableRadius)
{
	MoveTowardsActor(Target, FMoveFunction{}, NewAcceptableRadius);
}

void UARPGActorTowardsActorMovementComponent::MoveTowardsActor(AActor* Target, FMoveFunction AdditionalFunction,
                                                               float NewAcceptableRadius)
{
	TargetActor = Target;
	AcceptableRadius = NewAcceptableRadius;
	Move(AdditionalFunction);
}

void UARPGActorTowardsActorMovementComponent::MoveTowardsActorWithScale(AActor* Target)
{
	FMoveFunction NewAdditionalFunction;
	NewAdditionalFunction.BindDynamic(this, &UARPGActorTowardsActorMovementComponent::ScaleFunction);
	MoveTowardsActor(Target, NewAdditionalFunction);
}

// ReSharper disable once CppMemberFunctionMayBeConst
FTransform UARPGActorTowardsActorMovementComponent::ScaleFunction(float Time)
{
	FTransform Transform;
	Transform.SetScale3D(Transform.GetScale3D() * std::exp(-Time / 50));
	return Transform;
}
