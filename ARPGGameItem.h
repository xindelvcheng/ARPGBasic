// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGActor.h"
#include "ARPGActorMovement.h"
#include "ARPGCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "ARPGGameItem.generated.h"

class AARPGCharacter;
class UCollectableItemWidget;
class ACharacter;
class AARPGAction;
class UBillboardComponent;

UCLASS()
class AARPGGameItem : public AARPGCollectableObject
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UParticleSystemComponent* PromptFX;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UBillboardComponent* BillboardComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UARPGActorTowardsActorMovement* ActorMovementComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeTaken(AARPGCharacter* Character) { ; }

	UFUNCTION(BlueprintImplementableEvent,Category="GameItem",DisplayName="OnBeUsed")
	void BPFunc_OnBeUsed(AARPGCharacter* User);

	virtual void OnBeUsed(AARPGCharacter* User) { ; }
public:
	// Sets default values for this actor's properties
	AARPGGameItem();

	UFUNCTION(BlueprintCallable,Category="GameItem")
	AARPGGameItem* PickUpGameItem(AARPGCharacter* Character);


	UFUNCTION(BlueprintCallable,DisplayName="UseGameItem",Category="GameItem")
	bool TryToUseGameItem(AARPGCharacter* User);

	virtual UBoxComponent* GetBoxCollision() const
	{
		return BoxCollision;
	}

	virtual UParticleSystemComponent* GetPromptFX() const
	{
		return PromptFX;
	}

	virtual void SetPromptFX(UParticleSystemComponent* const NewPromptFX)
	{
		this->PromptFX = NewPromptFX;
	}

	virtual UBillboardComponent* GetBillboardComponent() const
	{
		return BillboardComponent;
	}
};
