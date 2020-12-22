// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGConfigSubsystem.generated.h"

class UARPGLockTargetWidget;
class UARPGPromptWidget;
class UGameItemWidget;
class UARPGNotifyWidget;
class UARPGStatusWidget;
/**
 * 
 */
UCLASS()
class  UARPGConfigSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	static UARPGConfigSubsystem* Get(UWorld* World); 

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	TSubclassOf<UARPGStatusWidget> StatusWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	TSubclassOf<UARPGNotifyWidget> NotifyWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	TSubclassOf<UGameItemWidget> GameItemWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	TSubclassOf<UARPGPromptWidget> PromptWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	TSubclassOf<UARPGLockTargetWidget> LockTargetWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGConfigSubsystem")
	UDataTable* AbilityConfigDataTable;
};
