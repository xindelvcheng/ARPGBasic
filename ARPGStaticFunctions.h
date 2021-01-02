// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARPGStaticFunctions.generated.h"

class AARPGCharacter;

UENUM()
enum class EChoice:uint8 { ChoiceA, ChoiceB };

/**
 * 工具函数库
 */
UCLASS()
class UARPGStaticFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void PrintLogToScreen(FString Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(FText Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(float Message, float Time = 5, FColor Color = FColor::Yellow);
	static void PrintLogToScreen(UObject* Message, float Time = 5, FColor Color = FColor::Yellow);

	DECLARE_DELEGATE_OneParam(FActorInitializeDelegate, AActor*);
	template <typename T>
	static T* SpawnActor(UClass* ActorClass, FTransform Transform, AARPGCharacter* OwnerCharacter,
	                     FActorInitializeDelegate ActorInitializeDelegate = {});

	template <typename T>
	static T* SpawnActor(AARPGCharacter* OwnerCharacter, FTransform Transform = {})
	{
		return SpawnActor<T>(T::StaticClass(), Transform, OwnerCharacter);
	}


	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(ExpandEnumAsExecs="Choice"))
	static void RandomChoice(float ChanceA, EChoice& Choice);

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",BlueprintPure)
	static FVector2D GetScreenSize();


	UFUNCTION(BlueprintCallable,Category="ARPGBASIC",BlueprintPure)
	static FTransform GetActorNearPositionTransform(AActor* OriginActor,
	                                                const FVector LocationOffset,
	                                                const FRotator RotationOffset);

	static FTimerHandle DelayDo(UWorld* World,FTimerDelegate TaskDelegate,float Delay);
};
