// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "ARPGMainCharacter.generated.h"

UCLASS()
class  AARPGMainCharacter : public AARPGCharacter
{
	GENERATED_BODY()

	//在AARPGPlayerController::OnPossess中赋值
	
public:
	// Sets default values for this character's properties
	AARPGMainCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	class UARPGGameItemsManagerComponent* GameItemsManagerComponent;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UARPGGameItemsManagerComponent* GetGameItemsManagerComponent() const
	{
		return GameItemsManagerComponent;
	}

	UPROPERTY(BlueprintReadWrite,Category="ARPGMainCharacter")
	APlayerController* MainPlayerController;
};
