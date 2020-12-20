// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSceneComponent.h"

#include "ARPGGameInstanceSubsystem.h"

// Sets default values for this component's properties
UARPGSceneComponent::UARPGSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARPGSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AARPGCharacter* Character = Cast<AARPGCharacter>(GetOwner()))
	{
		SetOwnerCharacter(Character);
	}
	else
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(
            FString::Printf(TEXT("%s的Owner和Instigator都未指定为一个ARPGCharacter，可能会引起错误！"), *GetFullName()));
	}
	
}


// Called every frame
void UARPGSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

