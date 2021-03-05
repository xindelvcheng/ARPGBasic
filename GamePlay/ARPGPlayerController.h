// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "ARPGPlayerController.generated.h"

class UARPGStatusWidget;

/**
 * 
 */
UCLASS()
class  AARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

	virtual void OnPossess(APawn* InPawn) override;

public:
	AARPGPlayerController();

	virtual void BeginPlay() override;
	
	DECLARE_DELEGATE(FPlayerControllerPossesEvent);
	FPlayerControllerPossesEvent OnPlayerControllerPossessed;
	TSubclassOf<UARPGStatusWidget> StatusWidgetClass;
};
