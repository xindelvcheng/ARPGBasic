// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArchiveStructs.h"
#include "CoreMinimal.h"

#include "ArchiveStructs.h"
#include "GameFramework/Character.h"
#include "GameFramework/SaveGame.h"
#include "APRGGameSaver.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class  UAPRGArchiveManager : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly)
    TArray<FArchiveInfoStruct> ArchiveInfos;
};


UCLASS(BlueprintType)
class  UAPRGGameSaver : public USaveGame
{
    GENERATED_BODY()

public:

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString ArchiveName = TEXT("空白存档");

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FTransform MainCharacterTransform;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic")
    TArray<FCharacterArchiveStruct> Characters;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Basic")
    TArray<FGameItemArchiveStruct> GameItems; 
    
    UPROPERTY()
    FName CurrentMap="MainMenu";

    UPROPERTY()
    FDateTime Time;
};
