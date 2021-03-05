// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSwordsManagerComponent.h"
#include "ARPGCharacter.h"
#include "ARPGSword.h"

// Sets default values for this component's properties
UARPGSwordsManagerComponent::UARPGSwordsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARPGSwordsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (AARPGSword* Sword : Swords)
	{
		Sword->SetOwner(GetOwner());
	}
}


// Called every frame
void UARPGSwordsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

