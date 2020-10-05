// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "APRGGameSaver.h"
#include "CharacterStatusComponent.h"
#include "CoreMinimal.h"

#include "ARPGMainCharacter.h"

#include "ARPGStatusWidget.h"
#include "GameItem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGGameInstanceSubsystem.generated.h"

class USaveGame;

UENUM()
enum class EChoice:uint8
{
    ChoiceA,
    ChoiceB
};


/**
 * 
 */
UCLASS()
class UARPGGameInstanceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    UPROPERTY()
    UAPRGArchiveManager* ArchiveManager;

    UPROPERTY()
    UAPRGGameSaver* GameSaver;
    
    UFUNCTION()
    void OnArchiveLoaded(const FString& SlotName, const int32 UserIndex, USaveGame* SaveGame);

    UFUNCTION()
    void OnLevelLoaded();

    

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;


public:
    UPROPERTY(BlueprintReadWrite,Category="ARPGBASIC")
    AARPGMainCharacter* MainCharacter;

    UPROPERTY(BlueprintReadWrite,Category="ARPGBASIC")
    class AARPGPlayerController* MainCharacterController;

    FString ArchiveManageSlot = TEXT("ArchiveManageSlot");
    int UserIndexInt32;

    UFUNCTION(BlueprintCallable)
    UAPRGArchiveManager* GetArchiveManager();

    UPROPERTY(BlueprintReadWrite,Category="ARPGBASIC")
    UARPGStatusWidget* StatusWidget;

    UFUNCTION(BlueprintCallable)
    void ShowNotify(UTexture2D* Icon, FText Title, FText Content) const;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC")
    FName CurrentStreamingLevelName = "";

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSaveGameEvent);

    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameSaving;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameSaveSuccess;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameLoading;
    UPROPERTY(BlueprintAssignable)
    FSaveGameEvent OnGameLoadSuccess;

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    bool SaveArchive(FString ArchiveName);


    UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
    void LoadArchive(FString ArchiveName);



    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadLevelEvent);

    UPROPERTY(BlueprintAssignable)
    FLoadLevelEvent OnLevelLoading;
    UPROPERTY(BlueprintAssignable)
    FLoadLevelEvent OnLevelLoadSuccess;

    UFUNCTION(BlueprintCallable,Category="ARPGBASIC",meta=(ExpandEnumAsExecs="Choice"))
    static void RandomChoice(float ChanceA, EChoice& Choice);

    static void PrintLogToScreen(FString Message,float Time = 5, FColor Color = FColor::Yellow);
};
