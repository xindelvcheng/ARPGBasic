// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActorComponent.h"


#include "ARPGCharacter.h"
#include "ARPGCoreSubsystem.h"

// Sets default values for this component's properties
UARPGActorComponent::UARPGActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}

AARPGCharacter* UARPGActorComponent::GetDirectlyOwnerCharacter() const
{
	AActor* Owner = GetOwner();
	while (Owner)
	{
		if (AARPGCharacter* GetOwnerCharacter = Cast<AARPGCharacter>(Owner))
		{
			return GetOwnerCharacter;
		}
		Owner = Owner->GetOwner();
	}
	return nullptr;
}


// Called when the game starts
void UARPGActorComponent::BeginPlay()
{
	Super::BeginPlay();
}



// Called every frame
void UARPGActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

