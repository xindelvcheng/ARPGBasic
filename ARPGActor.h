// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARPGActor.generated.h"

class AARPGCharacter;

UCLASS()
class AARPGActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	TWeakObjectPtr<AARPGCharacter> OwnerCharacter;

protected:
	virtual void PostActorCreated() override;
	
public:

	FORCEINLINE AARPGCharacter* GetOwnerCharacter() const { return OwnerCharacter.Get(); }
	virtual void SetOwnerCharacter(AARPGCharacter* NewOwner);

	// Sets default values for this actor's properties
	AARPGActor();

	void SetActorVisibility(bool bNewVisibility);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
