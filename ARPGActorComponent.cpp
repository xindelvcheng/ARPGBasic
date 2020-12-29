// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActorComponent.h"

#include "ARPGGameInstanceSubsystem.h"

// Sets default values for this component's properties
UARPGActorComponent::UARPGActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}


void UARPGActorComponent::SetOwnerCharacter(AARPGCharacter* NewOwner)
{
	OwnerCharacter = NewOwner; 
}

// Called when the game starts
void UARPGActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UARPGActorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	OwnerCharacter = GetOwner<AARPGCharacter>();
}


// Called every frame
void UARPGActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

