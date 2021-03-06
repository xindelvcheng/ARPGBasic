// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStaticFunctions.h"

#include "Kismet/KismetStringLibrary.h"

#include "ARPGCharacter.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGPlayerController.h"


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

void UARPGStaticFunctions::PrintLogToScreen(FVector Message, float Time, FColor Color)
{
	PrintLogToScreen(UKismetStringLibrary::Conv_VectorToString(Message), Time, Color);
}

void UARPGStaticFunctions::PrintLogToScreen(FRotator Message, float Time, FColor Color)
{
	PrintLogToScreen(UKismetStringLibrary::Conv_RotatorToString(Message), Time, Color);
}

void UARPGStaticFunctions::PrintLogToScreen(FTransform Message, float Time, FColor Color)
{
	PrintLogToScreen(UKismetStringLibrary::Conv_TransformToString(Message), Time, Color);
}

void UARPGStaticFunctions::PrintLogToScreen(UObject* Message, float Time, FColor Color)
{
	PrintLogToScreen(Message->GetFullName(), Time, Color);
}

void UARPGStaticFunctions::PrintMessageToLog(FString Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
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
	PrintLogToScreen(FString::Printf(TEXT("%s生成Actor出现错误"), *OwnerCharacter->GetName()));
	return nullptr;
}


FTransform UARPGStaticFunctions::ConvActorLocalTransformToWorldTransform(AActor* OriginActor,
                                                               const FVector& LocationOffset,
                                                               const FRotator& RotationOffset,
                                                               const FVector& RelativeScale)
{
	if (!OriginActor)
	{
		return FTransform{};
	}
	
	return FTransform{
		OriginActor->GetActorRotation() + RotationOffset,
		OriginActor->GetActorLocation() + OriginActor->GetActorForwardVector() * LocationOffset.X + OriginActor->
		GetActorRightVector() * LocationOffset.Y + OriginActor->GetActorUpVector() * LocationOffset.Z,
		OriginActor->GetActorScale3D() * RelativeScale
	};
}

FTimerHandle UARPGStaticFunctions::DelayDo(UWorld* World, FTimerDelegate TaskDelegate, float Delay)
{
	FTimerHandle TimerHandle;
	if (World)
	{
		World->GetTimerManager().SetTimer(TimerHandle, TaskDelegate, Delay, false);
	}
	else
	{
		PrintLogToScreen(TEXT("UARPGStaticFunctions::DelayDo:错误，World无效"));
	}
	return TimerHandle;
}

void UARPGStaticFunctions::SetInputMode(AController* Controller, EInputMode InputMode)
{
	/*此处省去了对参数判空，因为Cast<>(nullptr)的结果只会是nullptr*/
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		switch (InputMode)
		{
		case EInputMode::GameOnly:
			PlayerController->SetInputMode(FInputModeGameOnly{});
			PlayerController->SetShowMouseCursor(false);
			break;
		case EInputMode::UIOnly:
			PlayerController->SetInputMode(FInputModeUIOnly{});
			PlayerController->SetShowMouseCursor(true);
			break;
		case EInputMode::GameAndUI:
			PlayerController->SetInputMode(FInputModeGameAndUI{});
			PlayerController->SetShowMouseCursor(true);
			break;
		}
	}
}

void UARPGStaticFunctions::SetInputMode(UObject* WorldContextObject, EInputMode InputMode)
{
	if (APlayerController* MainPlayerController = UARPGCoreSubsystem::GetMainCharacterController(WorldContextObject))
	{
		SetInputMode(MainPlayerController, InputMode);
	}
}

FVector2D UARPGStaticFunctions::GetScreenSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		return GEngine->GameViewport->Viewport->GetSizeXY();
	}
	return FVector2D{1920, 1080};
}
