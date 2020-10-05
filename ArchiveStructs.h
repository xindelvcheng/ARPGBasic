#pragma once

#include "CoreMinimal.h"


#include "CharacterStatusComponent.h"

#include "ArchiveStructs.generated.h"

USTRUCT(BlueprintType)
struct FArchiveInfoStruct
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "UAPRGArchiveManager")
    FString SlotName;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "UAPRGArchiveManager")
    FString Timestamp;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "UAPRGArchiveManager")
    bool IsArchiveValid;
};

USTRUCT(BlueprintType)
struct FCharacterArchiveStruct
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    TSoftClassPtr<class AARPGCharacter> CharacterClassPath;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    FTransform CharacterTransform;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    FName CharacterName = "DefaultCharacter";

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int Level = 1;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int Coins;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int HealthSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int StaminaSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int AttackSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int DefenseSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int ToughnessSpecialty = 0;

    UPROPERTY(BlueprintReadWrite,Category="CharacterArchiveStruct")
    float CurrentHP;

    UPROPERTY(BlueprintReadWrite,Category="CharacterArchiveStruct")
    float CurrentSP;

    UPROPERTY(BlueprintReadWrite,Category="CharacterArchiveStruct")
    ECharacterState CharacterState;
};

USTRUCT(BlueprintType)
struct FCharacterInitConfigStruct : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    FName CharacterName = "DefaultCharacter";

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int Level = 10;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int Coins = 10;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int HealthSpecialty = 10;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int StaminaSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int AttackSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int DefenseSpecialty = 0;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterArchiveStruct")
    int ToughnessSpecialty = 0;
};


USTRUCT(BlueprintType)
struct FGameItemArchiveStruct
{
    GENERATED_BODY()

    UPROPERTY()
    TSoftClassPtr<class AGameItem> GameItemClass;

    UPROPERTY()
    int Number;
};