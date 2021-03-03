// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"
#include "ARPGCharacter.h"
#include "Components/ActorComponent.h"
#include "ARPGSwordsManagerComponent.generated.h"

class AARPGSword;
class UARPGDamageBoxComponent;



/**
 *剑灵管理组件
 */

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGSwordsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,Category="AARPGSword",meta=(AllowPrivateAccess))
	TArray<AARPGSword*> Swords;

public:
	// Sets default values for this component's properties
	UARPGSwordsManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
