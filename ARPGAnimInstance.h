// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ARPGAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	virtual void NativeBeginPlay() override;
protected:
	virtual bool HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent) override;
};
