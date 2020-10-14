// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterConfigPrimaryDataAsset.generated.h"

class AARPGAction;
class ATranscendentalLawsSystem;

/**
 * 
 */
UCLASS()
class UCharacterConfigPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	FName CharacterName = "DefaultCharacter";
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	FText CharacterDisplayName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int Level = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int Coins = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int HealthSpecialty = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int StaminaSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int AttackSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int DefenseSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterConfigPrimaryDataAsset")
	int ToughnessSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterConfigPrimaryDataAsset")
	TArray<TSubclassOf<AARPGAction>> MeleeAttackCollectionClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterConfigPrimaryDataAsset")
	TArray<TSubclassOf<AARPGAction>> RemoteAttackClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterConfigPrimaryDataAsset")
	TArray<TSubclassOf<AARPGAction>> AbilityClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterConfigPrimaryDataAsset")
	TArray<TSubclassOf<AARPGAction>> BuffClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterConfigPrimaryDataAsset")
	TArray<TSubclassOf<ATranscendentalLawsSystem>> TranscendentalLawsSystemClasses;
};
