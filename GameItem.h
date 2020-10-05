// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameItem.generated.h"

class ACharacter;
UCLASS()
class  AGameItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Custom")
	FName ItemName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Custom")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Custom")
	int Number=1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Custom")
	FText Note;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Custom")
	FText Description;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Custom")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Custom")
	UParticleSystemComponent* PromptFX;

    UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Custom")
    class UGameItemWidget* ItemWidget;
	
	UFUNCTION(BlueprintCallable)
	AGameItem* BeTaken();

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void BeUsed(ACharacter* User);

	UPROPERTY(BlueprintReadWrite)
	bool IsInBag;
};
