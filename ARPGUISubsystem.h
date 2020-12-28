// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGUISubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<UUserWidget*> UserWidgetsStack;

public:

	virtual TArray<UUserWidget*>& GetUserWidgetsStack()
	{
		return UserWidgetsStack;
	}
};
