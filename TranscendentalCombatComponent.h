// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacterCombatComponent.h"
#include "TranscendentalCombatComponent.generated.h"

class ATranscendentalLawsSystem;

/**
 * 
 */
UCLASS()
class UTranscendentalCombatComponent : public UARPGCharacterCombatComponent
{
	GENERATED_BODY()
	

protected:
	UPROPERTY()
	TArray<ATranscendentalLawsSystem*> TranscendentalLawsSystems;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
	TArray<TSubclassOf<ATranscendentalLawsSystem>> TranscendentalLawsSystemClasses;
	

public:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,Category="ARPGCharacterCombatComponent")
	ATranscendentalLawsSystem* CurrentActiveTranscendentalLawsSystem;

	UFUNCTION(BlueprintCallable,Category="TranscendentalCombatComponent")
	TArray<ATranscendentalLawsSystem*> GetAllTranscendentalLawsSystems()const{return TranscendentalLawsSystems;}

	UFUNCTION(BlueprintCallable,Category="TranscendentalCombatComponent")
	void ActivateTranscendentalLawsSystem(ATranscendentalLawsSystem* TranscendentalLawsSystem) ;

	UFUNCTION(BlueprintCallable,Category="TranscendentalCombatComponent")
    void DeActivateTranscendentalLawsSystem(ATranscendentalLawsSystem* TranscendentalLawsSystem) ;

	UFUNCTION(BlueprintCallable,Category="TranscendentalCombatComponent")
	void SwitchTranscendentalLawsSystem(ATranscendentalLawsSystem* TranscendentalLawsSystem);
};
