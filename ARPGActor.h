// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARPGActor.generated.h"

class AARPGCharacter;

UCLASS()
class TESTPROJECT_API AARPGActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly,meta=(AllowPrivateAccess))
	AARPGCharacter* OwnerCharacter;
	
public:

	FORCEINLINE AARPGCharacter* GetOwnerCharacter() const { return OwnerCharacter; }
	virtual void SetOwnerCharacter(AARPGCharacter* NewOwner) { OwnerCharacter = NewOwner; }
	
	// Sets default values for this actor's properties
	AARPGActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
