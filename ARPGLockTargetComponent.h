// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/ActorComponent.h"
#include "Components/WidgetComponent.h"

#include "ARPGLockTargetComponent.generated.h"

class AARPGCharacter;
class AARPGPlayerController;
class UARPGLockTargetWidget;

/*锁定组件，进行锁定的玩家和NPC都需要有这个组件。如果锁定图标的位置不对，考虑是否Widget锚点未设置成中心*/

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGLockTargetComponent : public UWidgetComponent
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere,Category="Debug",meta=(AllowPrivateAccess))
	bool bDrawDebug = false;
public:	
	// Sets default values for this component's properties
	UARPGLockTargetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AARPGCharacter* AttachedCharacter;
	AARPGPlayerController* MainCharacterPlayerController;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AARPGCharacter* LockingTarget;

	TArray<AARPGCharacter*> CharactersInSight;

	TArray<TEnumAsByte<	EObjectTypeQuery> > ObjectTypes = {ObjectTypeQuery3};
	FVector HalfSize{300,150,150};
	TArray<AActor*> ActorsToIgnore;

	TArray<AARPGCharacter*> Targets;
	TArray<AARPGCharacter*> LockedTargets;

	TArray<FHitResult> OutHits;

	UFUNCTION(BlueprintCallable,Category="ARPGLockTargetComponent")
	AARPGCharacter* ToggleLockTarget();

};
