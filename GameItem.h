// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"




#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameItem.generated.h"

class AARPGCharacter;
class UGameItemWidget;
class ACharacter;
class AARPGAction;

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
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	FName ItemName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	FText GameItemDisplayName;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	int Number=1;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	FText Introduction;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	FText Description;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="GameItem")
	TSubclassOf<AARPGAction> EffectActionClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItem")
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItem")
	UParticleSystemComponent* PromptFX;

    UPROPERTY(BlueprintReadWrite,Category="GameItem")
    UGameItemWidget* ItemWidget;

	UPROPERTY(BlueprintReadWrite,Category="GameItem")
	AARPGAction* EffectAction;
	
	UFUNCTION(BlueprintCallable,Category="GameItem")
	AGameItem* BeTaken();

	UFUNCTION(BlueprintImplementableEvent,Category="GameItem")
	void BeUsed(AARPGCharacter* User); 

	UFUNCTION(BlueprintCallable,DisplayName="UseGameItem",Category="GameItem")
    void NativeUseGameItem(AARPGCharacter* User); 
	
	UPROPERTY(BlueprintReadWrite,Category="GameItem")
	bool IsInBag;

	UPROPERTY(BlueprintReadWrite,Category="GameItem")
	bool IsUnique;
};
