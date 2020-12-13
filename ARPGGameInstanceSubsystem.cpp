// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameInstanceSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Engine/Classes/Engine/LevelStreaming.h"
#include "CoreUObject/Public/UObject/UObjectGlobals.h"


#include "APRGGameSaver.h"
#include "ARPGBasicSettings.h"
#include "GameItemWidget.h"
#include "ARPGPlayerController.h"
#include "ARPGGameItemsManagerComponent.h"

#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UARPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UARPGGameInstanceSubsystem::Tick));

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

void UARPGGameInstanceSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

AARPGMainCharacter* UARPGGameInstanceSubsystem::GetMainCharacter(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGGameInstanceSubsystem::Get(World))
	{
		return World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>()->MainCharacter.Get();
	}
	return nullptr;
}


AARPGPlayerController* UARPGGameInstanceSubsystem::GetMainCharacterController(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGGameInstanceSubsystem::Get(World))
	{
		return UARPGGameInstanceSubsystem::Get(World)->MainCharacterController.Get();
	}
	return nullptr;
}

UARPGStatusWidget* UARPGGameInstanceSubsystem::GetMainCharacterStatusWidget(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGGameInstanceSubsystem::Get(World))
	{
		return UARPGGameInstanceSubsystem::Get(World)->StatusWidget.Get();
	}
	return nullptr;
}

void UARPGGameInstanceSubsystem::ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title,
                                            FText Content)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (UARPGGameInstanceSubsystem::Get(World))
	{
		const auto StatusWidget = UARPGGameInstanceSubsystem::Get(World)->GetMainCharacterStatusWidget(World);
		if (StatusWidget)
		{
			StatusWidget->ShowNotify(Icon, Title, Content);
		}
		else
		{
			PrintLogToScreen(UKismetTextLibrary::Conv_TextToString(Title));
		}
	}
}

void UARPGGameInstanceSubsystem::RandomChoice(float chance, EChoice& Choice)
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

void UARPGGameInstanceSubsystem::PrintLogToScreen(FString Message, float Time, FColor Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Time, Color, Message);
	}
}

void UARPGGameInstanceSubsystem::PrintLogToScreen(FText Message, float Time, FColor Color)
{
	PrintLogToScreen(Message.ToString(), Time, Color);
}

void UARPGGameInstanceSubsystem::PrintLogToScreen(float Message, float Time, FColor Color)
{
	PrintLogToScreen(UKismetStringLibrary::Conv_FloatToString(Message), Time, Color);
}

void UARPGGameInstanceSubsystem::PrintLogToScreen(UObject* Message, float Time, FColor Color)
{
	PrintLogToScreen(Message->GetFullName(), Time, Color);
}

FTransform UARPGGameInstanceSubsystem::GetActorNearPositionTransform(AActor* OriginActor,
                                                                     const FVector LocationOffset,
                                                                     const FRotator RotationOffset)
{
	return FTransform{
		OriginActor->GetActorRotation() + RotationOffset,
		OriginActor->GetActorLocation() + OriginActor->GetActorForwardVector() * LocationOffset.X + OriginActor->
		GetActorRightVector() * LocationOffset.Y + OriginActor->GetActorUpVector() * LocationOffset.Z
	};
}

UARPGGameInstanceSubsystem* UARPGGameInstanceSubsystem::Get(UWorld* World)
{
	if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
	{
		return World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>();
	}

	return nullptr;
}

FVector2D UARPGGameInstanceSubsystem::GetScreenSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		return GEngine->GameViewport->Viewport->GetSizeXY();
	}
	return FVector2D{1920, 1080};
}

void UARPGGameInstanceSubsystem::MoveActorTowardsDirection(AActor* Actor, FVector Direction,
                                                           FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                           float Duration)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardsDirectionRecord::CreateRecord(Actor, Direction, MoveFinishDelegate, MoveRate,
		                                                             Duration);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGGameInstanceSubsystem::MoveActorTowardsDirectionFinishOnCollision(
	AActor* Actor, FVector Direction, TArray<AActor*> IgnoreActors,
	FCollisionDelegate OnCollision, float MoveRate, float Duration, bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}
	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardDirectionFinishOnCollision::CreateRecord(
			Actor, Direction, IgnoreActors, OnCollision, MoveRate, Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGGameInstanceSubsystem::MoveActorTowardDirectionFinishOnCollisionWithScale(AActor* Actor, FVector Direction,
	TArray<AActor*> IgnoreActors, FCollisionDelegate OnCollision, float MoveRate, float ScaleRate, float Duration,
	bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}
	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardDirectionFinishOnCollisionWithScale::CreateRecord(
			Actor, Direction, IgnoreActors, OnCollision, MoveRate, ScaleRate, Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGGameInstanceSubsystem::MoveActorTowardActor(AActor* Actor, AActor* Target,
                                                      FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                      float Duration, float AcceptableRadius)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardActorRecord::CreateRecord(Actor, Target, MoveFinishDelegate, MoveRate, Duration,
		                                                        AcceptableRadius);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGGameInstanceSubsystem::MoveActorTowardActorWithScale(AActor* Actor, AActor* Target,
                                                               FMoveFinishDelegate MoveFinishDelegate, float MoveRate,
                                                               float ScaleRate, float Duration, float AcceptableRadius)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorTowardActorWithScaleRecord::CreateRecord(
			Actor, Target, MoveFinishDelegate, MoveRate, ScaleRate,
			Duration, AcceptableRadius);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

void UARPGGameInstanceSubsystem::MoveActorComplex(AActor* Actor, FTransformFunctionOfTime TransformFunctionOfTime,
                                                  TArray<AActor*> IgnoreActors,
                                                  FCollisionDelegate OnCollision, float Duration,
                                                  bool ShouldStopAfterFirstCollision)
{
	if (!Actor)
	{
		return;
	}

	if (UARPGGameInstanceSubsystem* GameInstanceSubsystem = Get(Actor->GetWorld()))
	{
		auto Record = UMoveActorComplex::CreateRecord(Actor, TransformFunctionOfTime, IgnoreActors, OnCollision,
		                                              Duration, ShouldStopAfterFirstCollision);
		Record->MoveFinishEvent.AddDynamic(GameInstanceSubsystem, &UARPGGameInstanceSubsystem::BindToMoveFinish);
		GameInstanceSubsystem->MoveRecords.Emplace(Record);
	}
}

bool UARPGGameInstanceSubsystem::Tick(float DeltaSeconds)
{
	for (int i = MoveRecords.Num() - 1; i >= 0; --i)
	{
		MoveRecords[i]->Move(DeltaSeconds);
	}

	return true;
}
