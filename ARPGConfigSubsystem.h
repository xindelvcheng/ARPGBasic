// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGConfigSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class  UARPGConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGConfigSubsystem")
	class UDataTable* CharactersConfigDataTable;
};
