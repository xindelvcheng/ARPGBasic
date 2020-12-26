// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"
#include "ARPGSceneComponent.h"

#include "ARPGAimComponent.generated.h"

class AAimTargetActor;

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGAimComponent : public UARPGSceneComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGAimComponent",meta=(AllowPrivateAccess))
	AAimTargetActor* AimTargetActor;

public:
	UARPGAimComponent();

	

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset) override;

	virtual void Deactivate() override;

public:

	virtual AAimTargetActor* GetAimTargetActor() const
	{
		return AimTargetActor;
	}
};

UCLASS()
class AAimTargetActor : public AARPGActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	AAimTargetActor();
};