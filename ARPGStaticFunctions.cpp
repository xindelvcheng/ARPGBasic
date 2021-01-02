// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStaticFunctions.h"

#include "Kismet/KismetStringLibrary.h"

void UARPGStaticFunctions::PrintLogToScreen(FString Message, float Time, FColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Time, Color, Message);
	}
}

void UARPGStaticFunctions::PrintLogToScreen(FText Message, float Time, FColor Color)
{
	PrintLogToScreen(Message.ToString(), Time, Color);
}

void UARPGStaticFunctions::PrintLogToScreen(float Message, float Time, FColor Color)
{
	PrintLogToScreen(UKismetStringLibrary::Conv_FloatToString(Message), Time, Color);
}

void UARPGStaticFunctions::PrintLogToScreen(UObject* Message, float Time, FColor Color)
{
	PrintLogToScreen(Message->GetFullName(), Time, Color);
}