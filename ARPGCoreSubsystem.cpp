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






bool UARPGCoreSubsystem::Tick(float DeltaSeconds)
{
	return true;
}
