// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "ARPGActorMovement.generated.h"


UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGActorMovementComponent : public UActorComponent
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
	UARPGActorMovementComponent();

	/*以函数移动，函数的参数为t，返回值为FTransform（本地坐标系）*/
	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
	void Move(FMoveFunction NewMoveFunction);

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
    void MoveForward();

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
	void Stop();

	UFUNCTION()
	FTransform MoveForwardFunction(float Time);

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
class UARPGActorTowardsActorMovementComponent : public UARPGActorMovementComponent
{
	GENERATED_BODY()

	TWeakObjectPtr<AActor> TargetActor;
	float AcceptableRadius = 300;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
public:
	UARPGActorTowardsActorMovementComponent();

    void MoveTowardsActor(AActor* Target, float NewAcceptableRadius = 300);

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
	void MoveTowardsActor(AActor* Target, FMoveFunction AdditionalFunction, float NewAcceptableRadius = 300);

	UFUNCTION(BlueprintCallable,Category="ARPGActorMovement")
    void MoveTowardsActorWithScale(AActor* Target);
private:
	UFUNCTION()
	FTransform ScaleFunction(float Time);
};
