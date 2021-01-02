// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARPGStaticFunctions.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGStaticFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void PrintLogToScreen(FString Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(FText Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(float Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(UObject* Message, float Time = 5, FColor Color = FColor::Yellow);
};
