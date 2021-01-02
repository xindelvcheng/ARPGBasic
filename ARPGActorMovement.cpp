﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActorMovement.h"

#include <complex>


// Sets default values for this component's properties
UARPGActorTimeFunctionMovement::UARPGActorTimeFunctionMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UARPGActorTimeFunctionMovement::Move(FMoveFunction NewMoveFunction)
{
	Timer = 0;
	OriginTransform = GetOwner()->GetActorTransform();
	MoveFunction = NewMoveFunction;
}


// Called every frame
void UARPGActorTimeFunctionMovement::TickComponent(float DeltaTime, ELevelTick TickType,
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
				OriginTransform.GetLocation() + MoveFunction.Execute(Timer).GetLocation() * 100,
				OriginTransform.GetScale3D() * MoveFunction.Execute(Timer).GetScale3D()
			});
		};
	}
}

void UARPGActorTowardsActorMovement::TickComponent(float DeltaTime, ELevelTick TickType,
                                                   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetActor.IsValid())
	{
		GetOwner()->AddActorWorldOffset((TargetActor->GetActorLocation() - GetOwner()->GetActorLocation()) * DeltaTime);
		SetOriginTransform(GetOwner()->GetActorTransform());
	}
}

UARPGActorTowardsActorMovement::UARPGActorTowardsActorMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UARPGActorTowardsActorMovement::MoveTowardsActor(AActor* Target, float NewAcceptableRadius)
{
	MoveTowardsActor(Target,FMoveFunction{},NewAcceptableRadius);
}

void UARPGActorTowardsActorMovement::MoveTowardsActor(AActor* Target, FMoveFunction AdditionalFunction,
                                                      float NewAcceptableRadius)
{
	TargetActor = Target;
	AcceptableRadius = NewAcceptableRadius;
	Move(AdditionalFunction);
}

void UARPGActorTowardsActorMovement::MoveTowardsActorWithScale(AActor* Target)
{
	FMoveFunction NewAdditionalFunction;
	NewAdditionalFunction.BindDynamic(this,&UARPGActorTowardsActorMovement::ScaleFunction);
	MoveTowardsActor(Target,NewAdditionalFunction);
}

// ReSharper disable once CppMemberFunctionMayBeConst
FTransform UARPGActorTowardsActorMovement::ScaleFunction(float Time)
{
	FTransform Transform;
	Transform.SetScale3D( Transform.GetScale3D()*std::exp(-Time/50));
	return Transform;
}
