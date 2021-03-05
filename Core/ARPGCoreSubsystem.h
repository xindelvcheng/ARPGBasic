// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "APRGGameSaver.h"
#include "CoreMinimal.h"


#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGCoreSubsystem.generated.h"

class UARPGStatusWidget;
class AARPGPlayerController;
class AARPGMainCharacter;
class USaveGame;
class UActorMoveRecord;

/*TODO：构建事件路由*/
/*Instigator,EventName*/
DECLARE_MULTICAST_DELEGATE_TwoParams(FARPGEvent,UObject*,FName);

/**
* 管理游戏进程的子系统
*/
UCLASS()
class UARPGCoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	/*此三个变量在ARPG Player Controller的OnPosses回调中赋值，蓝图中可以获得*/
	TWeakObjectPtr<AARPGMainCharacter> MainCharacter;
	TWeakObjectPtr<AARPGPlayerController> MainCharacterController;
	TWeakObjectPtr<UARPGStatusWidget> StatusWidget;

	FARPGEvent ARPGEvent;


protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY()
	UUserWidget* LoadingWidget;
public:
	

	DECLARE_MULTICAST_DELEGATE_OneParam(FPreLoadMapDelegate, const FString& /* MapName */);
	FPreLoadMapDelegate PreLoadMap;
	DECLARE_MULTICAST_DELEGATE_OneParam(FPostLoadMapDelegate, UWorld* /* LoadedWorld */);
	FPostLoadMapDelegate PostLoadMap;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPreLoadMapEvent, const FString&, MapName);

	UPROPERTY(BlueprintAssignable,Category="ARPGGameInstanceSubsystem")
	FPreLoadMapEvent OnLoadingMap;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapLoadedEvent, UWorld*, LoadedWorld);

	UPROPERTY(BlueprintAssignable,Category="ARPGGameInstanceSubsystem")
	FMapLoadedEvent OnMapLoaded;

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
	))
	static AARPGMainCharacter* GetMainCharacter(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
	))
	static AARPGPlayerController* GetMainCharacterController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject
	))
	static UARPGStatusWidget* GetMainCharacterStatusWidget(const UObject* WorldContextObject);

	void SetMainCharacter(AARPGMainCharacter* NewMainCharacter);

	void SetMainCharacterController(AARPGPlayerController* NewMainCharacterController);

	void SetMainCharacterStatusWidget(UARPGStatusWidget* NewARPGStatusWidget);

	DECLARE_MULTICAST_DELEGATE(FSetupPlayerEvent)
	FSetupPlayerEvent OnPlayerSetupEnd;

	void SetupPlayer(AARPGMainCharacter* NewMainCharacter, AARPGPlayerController* NewMainCharacterController,
	                 UARPGStatusWidget* NewARPGStatusWidget);


	UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
	static void ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title, FText Content);
	

	static UARPGCoreSubsystem* Get(UWorld* World)
	{
		if (World && World->GetGameInstance())
		{
			return World->GetGameInstance()->GetSubsystem<UARPGCoreSubsystem>();
		}

		return nullptr;
	};


	UFUNCTION()
	bool Tick(float DeltaSeconds);
};


