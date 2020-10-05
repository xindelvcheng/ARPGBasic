// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGNonPlayerCharacter.h"

// Sets default values
AARPGNonPlayerCharacter::AARPGNonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AARPGNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AARPGNonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
