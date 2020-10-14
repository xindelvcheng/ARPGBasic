// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ARPGAction.h"
#include "ARPGMainCharacter.h"
#include "Tasks/AITask_MoveTo.h"



#include "ARPGAIController.generated.h"


/**
 * 
 */
UCLASS()
class TESTPROJECT_API AARPGAIController : public AAIController
{
	GENERATED_BODY()

protected:
	
	TWeakObjectPtr<class AARPGCharacter> ControlledCharacter;

	TWeakObjectPtr<class AARPGMainCharacter> MainCharacter;

	virtual void OnPossess(APawn* InPawn) override;
};

UCLASS()
class AAction_MoveToTarget : public AARPGAction
{
	GENERATED_BODY()

	
	AARPGCharacter* Target;
	float KeepDistance=150;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	
public:
	AAction_MoveToTarget();
	
	AARPGAIController* AIContoller;

	void SetKeepDistance(float NewDistance);

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void ActivateAction(AARPGCharacter* Target) override;
};

UCLASS()
class TESTPROJECT_API AARPGSimpleAIController : public AARPGAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPG AI",meta=(AllowPrivateAccess))
	float Threshold1=1500;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPG AI",meta=(AllowPrivateAccess))
	float Threshold2=700;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPG AI",meta=(AllowPrivateAccess))
	float Threshold3=300;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPG AI",meta=(AllowPrivateAccess))
	float Threshold4 =100;

	UPROPERTY()
	AAction_MoveToTarget* Action_MoveToTarget;

public:
	
	virtual void Tick(float DeltaSeconds) override;
	
};
