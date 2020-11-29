// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "APRGGameSaver.h"
#include "CharacterStatusComponent.h"
#include "CoreMinimal.h"

#include "ARPGMainCharacter.h"
#include "ARPGPlayerController.h"

#include "ARPGStatusWidget.h"
#include "GameItem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGArchiveSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGArchiveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UAPRGArchiveManager* ArchiveManager;

	UPROPERTY()
	UAPRGGameSaver* GameSaver;

	// UFUNCTION()
 //    void OnArchiveLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);

	UFUNCTION()
    void OnLevelLoaded();

public:

	static UARPGArchiveSubsystem* Get(UWorld* World);
	
	FString ArchiveManageSlot = TEXT("ArchiveManageSlot");
	int UserIndexInt32;

	UFUNCTION(BlueprintCallable)
    TArray<FArchiveInfoStruct> GetArchiveInfos() const;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
	FName CurrentStreamingLevelName = "";

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaveGameEvent);
	DECLARE_DYNAMIC_DELEGATE(FSaveGameDelegate);

	UPROPERTY(BlueprintAssignable)
	FSaveGameEvent OnGameSaving;
	UPROPERTY(BlueprintAssignable)
	FSaveGameEvent OnGameSaveSuccess;
	UPROPERTY(BlueprintAssignable)
	FSaveGameEvent OnGameLoading;
	UPROPERTY(BlueprintAssignable)
	FSaveGameEvent OnGameLoadSuccess;

	
    bool SaveArchive(FString ArchiveName, FSaveGameDelegate CompleteDelegate);

	UFUNCTION(BlueprintCallable,Category="ARPG Archive",meta=(WorldContext="WorldContextObject"),DisplayName="SaveArchive")
    static bool BPFunc_SaveArchive(UObject* WorldContextObject, int ArchiveIndex, FSaveGameDelegate CompleteDelegate);

    void LoadArchive(FString ArchiveName, FSaveGameDelegate CompleteDelegate);

	UFUNCTION(BlueprintCallable,Category="ARPG Archive",meta=(WorldContext="WorldContextObject"),DisplayName="LoadArchive")
    static void BPFunc_LoadArchive(UObject* WorldContextObject, int ArchiveIndex, FSaveGameDelegate CompleteDelegate);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadLevelEvent);

	UPROPERTY(BlueprintAssignable)
	FLoadLevelEvent OnLevelLoading;
	UPROPERTY(BlueprintAssignable)
	FLoadLevelEvent OnLevelLoadSuccess;
};
