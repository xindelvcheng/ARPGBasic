// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSceneComponent.h"

#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"

// Sets default values for this component's properties
UARPGSceneComponent::UARPGSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	// ...
}

void UARPGSceneComponent::SetOwnerCharacter(AARPGCharacter* NewOwner)
{
	if (!NewOwner)
	{
		UARPGStaticFunctions::PrintLogToScreen(
            FString::Printf(TEXT("%s的Owner不是一个ARPGCharacter，可能会引起错误！"), *GetFullName()));
	}
	OwnerCharacter = NewOwner;
}

void UARPGSceneComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetOwnerCharacter(GetOwner<AARPGCharacter>());
}

// Called when the game starts
void UARPGSceneComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UARPGSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
