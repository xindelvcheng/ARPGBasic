// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGActorComponent.generated.h"

class AARPGCharacter;

UCLASS(ClassGroup=(ARPGBasic),meta=(BlueprintSpawnableComponent))
class TESTPROJECT_API UARPGActorComponent : public UActorComponent
{
	GENERATED_BODY()

	
	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	TWeakObjectPtr<AARPGCharacter> OwnerCharacter;
public:	
	// Sets default values for this component's properties
	UARPGActorComponent();

protected:

	FORCEINLINE AARPGCharacter* GetOwnerCharacter() const { return OwnerCharacter.Get(); }
	virtual void SetOwnerCharacter(AARPGCharacter* NewOwner);
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void InitializeComponent() override;

	
public:
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
