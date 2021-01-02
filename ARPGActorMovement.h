// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActorComponent.h"
#include "Components/ActorComponent.h"
#include "ARPGActorMovement.generated.h"


UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGActorTimeFunctionMovement : public UActorComponent
{
	GENERATED_BODY()

	float Timer;
	FTransform OriginTransform;

public:
	DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FTransform, FMoveFunction, float, Time);

private:
	FMoveFunction MoveFunction;

public:
	// Sets default values for this component's properties
	UARPGActorTimeFunctionMovement();
	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
	void Move(FMoveFunction NewMoveFunction);


	virtual float GetTimer()
	{
		return Timer;
	}

	virtual void SetTimer(const float NewTimer)
	{
		this->Timer = NewTimer;
	}

	virtual FTransform GetOriginTransform()
	{
		return OriginTransform;
	}

	virtual void SetOriginTransform(const FTransform& NewOriginTransform)
	{
		this->OriginTransform = NewOriginTransform;
	}

protected:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGActorTowardsActorMovement : public UARPGActorTimeFunctionMovement
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> TargetActor;
	float AcceptableRadius = 300;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	UARPGActorTowardsActorMovement();

    void MoveTowardsActor(AActor* Target, float NewAcceptableRadius = 300);

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
	void MoveTowardsActor(AActor* Target, FMoveFunction AdditionalFunction, float NewAcceptableRadius = 300);

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
    void MoveTowardsActorWithScale(AActor* Target);
private:
	UFUNCTION()
	FTransform ScaleFunction(float Time);
};
