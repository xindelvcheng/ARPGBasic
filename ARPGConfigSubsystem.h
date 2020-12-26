// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"

#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGConfigSubsystem.generated.h"

class AAimTargetActor;
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

	UPROPERTY(BlueprintReadOnly,Category="ARPG Core Config")
	UDataTable* AbilityConfigDataTable;

	UPROPERTY(BlueprintReadOnly,Category="ARPG GUI Config")
	TSubclassOf<UARPGStatusWidget> StatusWidgetClass;

	UPROPERTY(BlueprintReadOnly,Category="ARPG GUI Config")
	TSubclassOf<UARPGNotifyWidget> NotifyWidgetClass;

	UPROPERTY(BlueprintReadOnly,Category="ARPG GUI Config")
	TSubclassOf<UGameItemWidget> GameItemWidgetClass;

	UPROPERTY(BlueprintReadOnly,Category="ARPG GUI Config")
	TSubclassOf<UARPGPromptWidget> PromptWidgetClass;

	UPROPERTY(BlueprintReadOnly,Category="ARPG GUI Config")
	TSubclassOf<UARPGLockTargetWidget> LockTargetWidgetClass;
	
	UPROPERTY(BlueprintReadWrite,Category="ARPG GUI Config")
	TSubclassOf<AAimTargetActor> AimPromptActorClass;

	UPROPERTY(BlueprintReadOnly,Category="ARPG Art Resource Config")
	UTexture2D* DefaultGameplayIcon;

	UPROPERTY(BlueprintReadOnly,Category="ARPG Art Resource Config")
	UTexture2D* DefaultGameItemIcon;

	UPROPERTY(BlueprintReadOnly,Category="ARPG Art Resource Config")
	UTexture2D* DefaultAbilityIcon;
};
