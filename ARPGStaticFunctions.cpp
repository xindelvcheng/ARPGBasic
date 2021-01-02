// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStaticFunctions.h"


#include "ARPGCharacter.h"
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

void UARPGStaticFunctions::RandomChoice(float chance, EChoice& Choice)
{
	if (chance >= FMath::FRand())
	{
		Choice = EChoice::ChoiceA;
	}
	else
	{
		Choice = EChoice::ChoiceB;
	}
}

template <typename T>
T* UARPGStaticFunctions::SpawnActor(UClass* ActorClass, FTransform Transform, AARPGCharacter* OwnerCharacter,
                                    FActorInitializeDelegate ActorInitializeDelegate)
{
	if (T* Actor = OwnerCharacter->GetWorld()->SpawnActorDeferred<T>(
		ActorClass, Transform, OwnerCharacter, OwnerCharacter,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
	{
		ActorInitializeDelegate.ExecuteIfBound(Actor);
		Actor->FinishSpawning(Transform);
		return Actor;
	}
	UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("%s生成Actor出现错误"), *OwnerCharacter->GetName()));
	return nullptr;
}


FTransform UARPGStaticFunctions::GetActorNearPositionTransform(AActor* OriginActor,
                                                               const FVector LocationOffset,
                                                               const FRotator RotationOffset)
{
	return FTransform{
		OriginActor->GetActorRotation() + RotationOffset,
		OriginActor->GetActorLocation() + OriginActor->GetActorForwardVector() * LocationOffset.X + OriginActor->
		GetActorRightVector() * LocationOffset.Y + OriginActor->GetActorUpVector() * LocationOffset.Z
	};
}

FTimerHandle UARPGStaticFunctions::DelayDo(UWorld* World, FTimerDelegate TaskDelegate, float Delay)
{
	FTimerHandle TimerHandle;
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandle,TaskDelegate,Delay,false);
	}else
	{
		PrintLogToScreen(TEXT("UARPGStaticFunctions::DelayDo:错误，World无效"));
	}
	return TimerHandle;
}

FVector2D UARPGStaticFunctions::GetScreenSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		return GEngine->GameViewport->Viewport->GetSizeXY();
	}
	return FVector2D{1920, 1080};
}
