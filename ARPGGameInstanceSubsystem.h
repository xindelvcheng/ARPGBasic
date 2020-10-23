// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "APRGGameSaver.h"
#include "CharacterStatusComponent.h"
#include "CoreMinimal.h"

#include "ARPGMainCharacter.h"
#include "ARPGPlayerController.h"

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

    UPROPERTY()
    TArray<FArchiveInfoStruct> ArchiveInfos;
    
protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;


public:
    FString ArchiveManageSlot = TEXT("ArchiveManageSlot");
    int UserIndexInt32;

    UFUNCTION(BlueprintCallable)
    TArray<FArchiveInfoStruct> GetArchiveInfos() const;

private:
    /*此三个变量在ARPG Player Controller的OnPosses回调中赋值，蓝图中可以获得*/
    TWeakObjectPtr<AARPGMainCharacter> MainCharacter;
    TWeakObjectPtr<AARPGPlayerController> MainCharacterController;
    TWeakObjectPtr<UARPGStatusWidget> StatusWidget;

public:
    
    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject))
    static AARPGMainCharacter* GetMainCharacter(const UObject* WorldContextObject) ;

    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject))
    static AARPGPlayerController* GetMainCharacterController(const UObject* WorldContextObject) ;

    UFUNCTION(BlueprintCallable,BlueprintPure,Category="ARPGGameInstanceSubsystem",meta=(WorldContext=WorldContextObject))
    static UARPGStatusWidget* GetMainCharacterStatusWidget(const UObject* WorldContextObject);

    void SetMainCharacter(AARPGMainCharacter* NewMainCharacter){MainCharacter = NewMainCharacter;}
    void SetMainCharacterController(AARPGPlayerController* NewMainCharacterController){MainCharacterController = NewMainCharacterController;}
    void SetMainCharacterStatusWidget(UARPGStatusWidget* NewARPGStatusWidget){StatusWidget = NewARPGStatusWidget;}

    DECLARE_MULTICAST_DELEGATE(FSetupPlayerEvent)
    FSetupPlayerEvent OnPlayerSetupEnd;
    
    void SetupPlayer(AARPGMainCharacter* NewMainCharacter,AARPGPlayerController* NewMainCharacterController,UARPGStatusWidget* NewARPGStatusWidget)
    {
        MainCharacter = NewMainCharacter;
        MainCharacterController = NewMainCharacterController;
        StatusWidget = NewARPGStatusWidget;
        OnPlayerSetupEnd.Broadcast();
    }
    
public:

    UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
    static void ShowNotify(const UObject* WorldContextObject, UTexture2D* Icon, FText Title, FText Content);

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

    static void PrintLogToScreen(FString Message, float Time = 5, FColor Color = FColor::Yellow);


    static UARPGGameInstanceSubsystem* Get(UWorld* World);

};


