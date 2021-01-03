// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"
#include "ARPGSceneComponent.h"

#include "ARPGAimComponent.generated.h"

class AAimTargetActor;

//TODO：将瞄准组件修改为围绕主角周围的一个圈的方向，释放法术（法术向前移动）

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGAimComponent : public UARPGSceneComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGAimComponent",meta=(AllowPrivateAccess))
	AAimTargetActor* AimTargetActor;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGAimComponent",meta=(AllowPrivateAccess))
	TSubclassOf<AAimTargetActor> AimPromptActorClass;

	bool bAimTargetResultIsValid;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGAimComponent",meta=(AllowPrivateAccess))
	float AimTargetResetDuration = 15;

	FTimerHandle ResetAimTargetTimerHandle;

	/*瞄准光圈的半径*/
	float AimRadius = 500;

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


	virtual bool IsAimTargetResultIsValid() const
	{
		return bAimTargetResultIsValid;
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
