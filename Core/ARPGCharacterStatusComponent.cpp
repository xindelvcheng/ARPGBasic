// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacterStatusComponent.h"

#include "ARPGCharacter.h"


// Sets default values for this component's properties
UARPGCharacterStatusComponent::UARPGCharacterStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UARPGCharacterStatusComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void UARPGCharacterStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UARPGCharacterStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

