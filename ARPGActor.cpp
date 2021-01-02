// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActor.h"

#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"

void AARPGActor::PostActorCreated()
{
	Super::PostActorCreated();

	if (!OwnerCharacter.IsValid())
	{
		AARPGCharacter* Character = Cast<AARPGCharacter>(GetInstigator());
		if (!Character)
		{
			Character = Cast<AARPGCharacter>(GetOwner());
			if (!Character)
			{
				UARPGStaticFunctions::PrintLogToScreen(
                    FString::Printf(TEXT("%s的Owner和Instigator都未指定为一个ARPGCharacter，可能会引起错误！"), *GetFullName()));
				
			}
		}
		SetOwnerCharacter(Character);
	}
}

void AARPGActor::SetOwnerCharacter(AARPGCharacter* NewOwner)
{
	OwnerCharacter = NewOwner; 
}

// Sets default values
AARPGActor::AARPGActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AARPGActor::SetActorVisibility(bool bNewVisibility)
{
	TArray<USceneComponent*> Components;
	GetComponents<USceneComponent>(Components);
	for (USceneComponent* Component : Components)
	{
		Component->SetVisibility(bNewVisibility);
	}
}

// Called when the game starts or when spawned
void AARPGActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AARPGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
