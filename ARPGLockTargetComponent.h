// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/ActorComponent.h"
#include "ARPGLockTargetComponent.generated.h"

class AARPGCharacter;
class AARPGPlayerController;
class UARPGLockTargetWidget;

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGLockTargetComponent : public UActorComponent
{
	GENERATED_BODY()

	
public:	
	// Sets default values for this component's properties
	UARPGLockTargetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AARPGCharacter* AttachedCharacter;
	AARPGPlayerController* MainCharacterPlayerController;
	UARPGLockTargetWidget* LockTargetWidget;
	FVector2D LockIconScreenPosition;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AARPGCharacter* LockingTarget;

	TArray<AARPGCharacter*> CharactersInSight;

	TArray<TEnumAsByte<	EObjectTypeQuery> > ObjectTypes = {ObjectTypeQuery3};
	FVector HalfSize = FVector(300,150,150);
	TArray<AActor*> ActorsToIgnore;
	TArray<AARPGCharacter*> Targets;
	TArray<AARPGCharacter*> LockedTargets;

	TArray<FHitResult> OutHits;

	UFUNCTION(BlueprintCallable,Category="ARPGLockTargetComponent")
	AARPGCharacter* ToggleLockTarget();

};
