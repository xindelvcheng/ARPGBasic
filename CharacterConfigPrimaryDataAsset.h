// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ARPGAction.h"
#include "ARPGCharacter.h"
#include "ARPGCastAction.h"
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
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Basic Information")
	FName CharacterName = "DefaultCharacter";
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Basic Information")
	FText CharacterDisplayName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int Level = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int Coins = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int HealthSpecialty = 10;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int StaminaSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int AttackSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int DefenseSpecialty = 0;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Character Property")
	int ToughnessSpecialty = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Combat")
	TArray<FMeleeAttackActionDescriptionStruct> MeleeAttacks;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Combat")
	TArray<TSubclassOf<AARPGCastAction>> AbilityClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Combat")
	TArray<FName> SpellNames;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Combat")
	TArray<TSubclassOf<AARPGBuff>> BuffClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Combat")
	TArray<TSubclassOf<ATranscendentalLawsSystem>> TranscendentalLawsSystemClasses;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Art Resource")
	UAnimSequence* DeathAnimationAsset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Art Resource")
	TArray<UAnimMontage*> HitReactAnimMontages;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Art Resource")
	TArray<FGroundTypeFootstepSoundPairStruct> FootstepSoundEffects;
};
