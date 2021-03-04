// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <functional>

#include "Components/ActorComponent.h"
#include "ARPGActorComponent.generated.h"

class AARPGCharacter;


UCLASS(ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent))
class TESTPROJECT_API UARPGActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UARPGActorComponent();

	AARPGCharacter* GetDirectlyOwnerCharacter() const ;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
