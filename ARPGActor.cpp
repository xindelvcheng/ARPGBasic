// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGActor.h"

#include "ARPGGameInstanceSubsystem.h"

// Sets default values
AARPGActor::AARPGActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AARPGActor::BeginPlay()
{
	Super::BeginPlay();
	if (!OwnerCharacter)
	{
		AARPGCharacter* Character = Cast<AARPGCharacter>(GetOwner());
		if (!Character)
		{
			Character = Cast<AARPGCharacter>(GetInstigator());
			if (Character)
			{
				SetOwnerCharacter(Character);
			}else
			{
				UARPGGameInstanceSubsystem::PrintLogToScreen(
                    FString::Printf(TEXT("%s的Owner和Instigator都未指定为一个ARPGCharacter，可能会引起错误！"), *GetFullName()));
			}
		}
	}
	
}

// Called every frame
void AARPGActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
