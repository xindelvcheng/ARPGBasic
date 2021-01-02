// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCoreSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Engine/Classes/Engine/LevelStreaming.h"
#include "CoreUObject/Public/UObject/UObjectGlobals.h"


#include "APRGGameSaver.h"
#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "CollectableItemWidget.h"
#include "ARPGPlayerController.h"
#include "ARPGCollectionComponent.h"
#include "ARPGStaticFunctions.h"

#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UARPGCoreSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UARPGCoreSubsystem::Tick));

	FCoreUObjectDelegates::PreLoadMap.AddLambda([&](const FString& MapName)
	{
		if (UARPGBasicSettings::Get())
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0))
			{
				LoadingWidget = CreateWidget(PlayerController,UARPGBasicSettings::Get()->LoadingWidgetClass.LoadSynchronous());
				LoadingWidget->AddToViewport();
            }
		}
		PreLoadMap.Broadcast(MapName);
		OnLoadingMap.Broadcast(MapName);
	});

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddLambda([&](UWorld* LoadedWorld)
	{
		if (LoadingWidget)
		{
			LoadingWidget->RemoveFromParent();
		}
		
		PostLoadMap.Broadcast(LoadedWorld);
		OnMapLoaded.Broadcast(LoadedWorld);
	});
}

void UARPGCoreSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

AARPGMainCharacter* UARPGCoreSubsystem::GetMainCharacter(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGCoreSubsystem::Get(World))
	{
		return World->GetGameInstance()->GetSubsystem<UARPGCoreSubsystem>()->MainCharacter.Get();
	}
	return nullptr;
}


AARPGPlayerController* UARPGCoreSubsystem::GetMainCharacterController(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGCoreSubsystem::Get(World))
	{
		return UARPGCoreSubsystem::Get(World)->MainCharacterController.Get();
	}
	return nullptr;
}

UARPGStatusWidget* UARPGCoreSubsystem::GetMainCharacterStatusWidget(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGCoreSubsystem::Get(World))
	{
		return UARPGCoreSubsystem::Get(World)->StatusWidget.Get();
	}
	return nullptr;
}

void UARPGCoreSubsystem::SetMainCharacter(AARPGMainCharacter* NewMainCharacter)
{
	MainCharacter = NewMainCharacter; 
}

void UARPGCoreSubsystem::SetMainCharacterController(AARPGPlayerController* NewMainCharacterController)
{
	MainCharacterController = NewMainCharacterController;
}

void UARPGCoreSubsystem::SetMainCharacterStatusWidget(UARPGStatusWidget* NewARPGStatusWidget)
{
	StatusWidget = NewARPGStatusWidget;
}

void UARPGCoreSubsystem::SetupPlayer(AARPGMainCharacter* NewMainCharacter,
	AARPGPlayerController* NewMainCharacterController, UARPGStatusWidget* NewARPGStatusWidget)
{
	MainCharacter = NewMainCharacter;
	MainCharacterController = NewMainCharacterController;
	StatusWidget = NewARPGStatusWidget;
	OnPlayerSetupEnd.Broadcast();
}

void UARPGCoreSubsystem::ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title,
                                            FText Content)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGCoreSubsystem::Get(World))
	{
		const auto StatusWidget = UARPGCoreSubsystem::Get(World)->GetMainCharacterStatusWidget(World);
		if (StatusWidget)
		{
			StatusWidget->ShowNotify(Icon, Title, Content);
		}
		else
		{
			UARPGStaticFunctions::PrintLogToScreen(UKismetTextLibrary::Conv_TextToString(Title));
		}
	}
}

template <typename T>
T* UARPGCoreSubsystem::SpawnActor(UClass* ActorClass, FTransform Transform, AARPGCharacter* OwnerCharacter,
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

void UARPGCoreSubsystem::RandomChoice(float chance, EChoice& Choice)
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



FTransform UARPGCoreSubsystem::GetActorNearPositionTransform(AActor* OriginActor,
                                                                     const FVector LocationOffset,
                                                                     const FRotator RotationOffset)
{
	return FTransform{
		OriginActor->GetActorRotation() + RotationOffset,
		OriginActor->GetActorLocation() + OriginActor->GetActorForwardVector() * LocationOffset.X + OriginActor->
		GetActorRightVector() * LocationOffset.Y + OriginActor->GetActorUpVector() * LocationOffset.Z
	};
}

FVector2D UARPGCoreSubsystem::GetScreenSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		return GEngine->GameViewport->Viewport->GetSizeXY();
	}
	return FVector2D{1920, 1080};
}

void UARPGCoreSubsystem::MoveActorTowardsDirection(AActor* Actor, FVector Direction,
                                                           FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                           float Duration)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardsDirectionRecord::CreateRecord(Actor, Direction, MoveFinishDelegate, MoveRate,
		                                                             Duration);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGCoreSubsystem::MoveActorTowardsDirectionFinishOnCollision(
	AActor* Actor, FVector Direction, TArray<AActor*> IgnoreActors,
	FCollisionDelegate OnCollision, float MoveRate, float Duration, bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}
	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardDirectionFinishOnCollision::CreateRecord(
			Actor, Direction, IgnoreActors, OnCollision, MoveRate, Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGCoreSubsystem::MoveActorTowardDirectionFinishOnCollisionWithScale(AActor* Actor, FVector Direction,
	TArray<AActor*> IgnoreActors, FCollisionDelegate OnCollision, float MoveRate, float ScaleRate, float Duration,
	bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}
	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardDirectionFinishOnCollisionWithScale::CreateRecord(
			Actor, Direction, IgnoreActors, OnCollision, MoveRate, ScaleRate, Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGCoreSubsystem::MoveActorTowardActor(AActor* Actor, AActor* Target,
                                                      FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                      float Duration, float AcceptableRadius)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardActorRecord::CreateRecord(Actor, Target, MoveFinishDelegate, MoveRate, Duration,
		                                                        AcceptableRadius);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGCoreSubsystem::MoveActorTowardActorWithScale(AActor* Actor, AActor* Target,
                                                               FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                               float ScaleRate, float Duration, float AcceptableRadius)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardActorWithScaleRecord::CreateRecord(
			Actor, Target, MoveFinishDelegate, MoveRate, ScaleRate,
			Duration, AcceptableRadius);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGCoreSubsystem::MoveActorComplex(AActor* Actor, FTransformFunctionOfTime TransformFunctionOfTime,
                                                  TArray<AActor*> IgnoreActors,
                                                  FCollisionDelegate OnCollision, float Duration,
                                                  bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGCoreSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorComplex::CreateRecord(Actor, TransformFunctionOfTime, IgnoreActors, OnCollision,
		                                              Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGCoreSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

bool UARPGCoreSubsystem::Tick(float DeltaSeconds)
{
	for (int i = MoveRecords.Num() - 1; i >= 0; --i)
	{
		MoveRecords[i]->Move(DeltaSeconds);
	}

	return true;
}
