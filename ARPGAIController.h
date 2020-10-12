// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ARPGMainCharacter.h"


#include "ARPGAIController.generated.h"


/**
 * 
 */
UCLASS()
class TESTPROJECT_API AARPGAIController : public AAIController
{
	GENERATED_BODY()

protected:
	
	TWeakObjectPtr<class AARPGCharacter> ControlledCharacter;

	TWeakObjectPtr<class AARPGMainCharacter> MainCharacter;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaSeconds) override;
};
