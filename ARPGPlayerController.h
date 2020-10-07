// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGStatusWidget.h"
#include "GameFramework/PlayerController.h"
#include "ARPGPlayerController.generated.h"

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
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerControllerPossesEvent,ACharacter*,PossedCharacter);
	FPlayerControllerPossesEvent OnPlayerControllerPosses;
	TSubclassOf<UARPGStatusWidget> StatusWidgetClass;
};
