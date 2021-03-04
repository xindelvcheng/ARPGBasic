// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGActorComponent.generated.h"

class AARPGCharacter;

UCLASS(ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent))
class TESTPROJECT_API UARPGActorComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UARPGActorComponent();

	AARPGCharacter* GetOwnerCharacter() const { return GetOwner<AARPGCharacter>(); }

protected:


	// Called when the game starts
	virtual void BeginPlay() override;


public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
