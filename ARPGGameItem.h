// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGActor.h"
#include "Components/BoxComponent.h"
#include "ARPGGameItem.generated.h"

class AARPGCharacter;
class UGameItemWidget;
class ACharacter;
class AARPGAction;
class UBillboardComponent;

UCLASS()
class AARPGGameItem : public AARPGActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UParticleSystemComponent* PromptFX;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="GameItem Basic Component",meta=(AllowPrivateAccess))
	UBillboardComponent* BillboardComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FName ItemName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText GameItemDisplayName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	int Number = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText Introduction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText Description;
	
	UPROPERTY(BlueprintReadOnly,Category="GameItem Flag",meta=(AllowPrivateAccess))
	bool IsInBag;

	UPROPERTY(BlueprintReadOnly,Category="GameItem Flag",meta=(AllowPrivateAccess))
	bool IsUnique;

	UPROPERTY(BlueprintReadOnly,Category="GameItem",meta=(AllowPrivateAccess))
	UGameItemWidget* ItemWidget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnBeTaken(AARPGCharacter* Character);
public:
	// Sets default values for this actor's properties
	AARPGGameItem();
	
	UFUNCTION(BlueprintCallable,Category="GameItem")
	AARPGGameItem* PickUpGameItem(AARPGCharacter* Character);

	UFUNCTION()
	void BindToHasBeenTaken();

	UFUNCTION(BlueprintImplementableEvent,Category="GameItem")
	void BPFunc_OnBeUsed(AARPGCharacter* User);

	
    virtual void OnBeUsed(AARPGCharacter* User);
	
	UFUNCTION(BlueprintCallable,DisplayName="UseGameItem",Category="GameItem")
	void UseGameItem(AARPGCharacter* User);

	virtual const FName& GetItemName() const
	{
		return ItemName;
	}

	virtual void SetItemName(const FName& NewItemName)
	{
		this->ItemName = NewItemName;
	}

	virtual const FText& GetGameItemDisplayName() const
	{
		return GameItemDisplayName;
	}

	virtual void SetGameItemDisplayName(const FText& NewGameItemDisplayName)
	{
		this->GameItemDisplayName = NewGameItemDisplayName;
	}

	virtual UTexture2D* GetItemIcon() const
	{
		return ItemIcon;
	}

	virtual void SetItemIcon(UTexture2D* const NewItemIcon)
	{
		this->ItemIcon = NewItemIcon;
	}

	virtual int GetNumber() const
	{
		return Number;
	}

	virtual void SetNumber(const int NewNumber)
	{
		this->Number = NewNumber;
	}

	virtual void UpdateNumber(const int DeltaNumber)
	{
		this->Number += DeltaNumber;
	}


	virtual const FText& GetIntroduction() const
	{
		return Introduction;
	}

	virtual void SetIntroduction(const FText& NewIntroduction)
	{
		this->Introduction = NewIntroduction;
	}

	virtual const FText& GetDescription() const
	{
		return Description;
	}

	virtual void SetDescription(const FText& NewDescription)
	{
		this->Description = NewDescription;
	}
	
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

	virtual UGameItemWidget* GetItemWidget() const
	{
		return ItemWidget;
	}

	virtual void SetItemWidget(UGameItemWidget* const NewItemWidget)
	{
		this->ItemWidget = NewItemWidget;
	}

	virtual bool GetIsInBag() const
	{
		return IsInBag;
	}

	virtual void SetIsInBag(const bool bGameItemIsInBag)
	{
		IsInBag = bGameItemIsInBag;
	}

	virtual bool GetIsUnique() const
	{
		return IsUnique;
	}

	virtual void SetIsUnique(const bool bGameItemIsUnique)
	{
		IsUnique = bGameItemIsUnique;
	}
};
