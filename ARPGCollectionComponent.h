// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "ARPGActor.h"
#include "ARPGActorComponent.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetTextLibrary.h"

#include "ARPGCollectionComponent.generated.h"


class AARPGCharacter;
class AARPGGameItem;
class AARPGCastAction;

UCLASS( ClassGroup=(ARPGBasic))
class AARPGCollectableObject : public AARPGActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FName ItemName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText GameItemDisplayName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	int ItemNum = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText ItemIntroduction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="GameItem Settings",meta=(AllowPrivateAccess))
	FText ItemDescription;

	DECLARE_MULTICAST_DELEGATE_OneParam(FItemChangeEvent,AARPGCollectableObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FItemExhaustEvent,AARPGCollectableObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FItemBeSelectedEvent,AARPGCollectableObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FItemBeSelectedEvent,AARPGCollectableObject*);
	
	FItemChangeEvent ItemChangedEvent;
	FItemExhaustEvent ItemExhaustEvent;
	FItemBeSelectedEvent ItemBeSelectedEvent;
	FItemBeSelectedEvent ItemDeselectedEvent;
public:
	AARPGCollectableObject();

	void SelectItem()
	{
		ItemBeSelectedEvent.Broadcast(this);
	};
	
	void DeselectItem()
	{
		ItemDeselectedEvent.Broadcast(this);
	};
	
	
	virtual const FName& GetItemName() const
	{
		return ItemName;
	}

	virtual void SetItemName(const FName& NewItemName)
	{
		this->ItemName = NewItemName;
		ItemChangedEvent.Broadcast(this);
	}

	virtual const FText& GetGameItemDisplayName() const
	{
		return GameItemDisplayName;
	}

	virtual void SetItemDisplayName(const FText& NewGameItemDisplayName)
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
		ItemChangedEvent.Broadcast(this);
	}

	virtual int GetItemNumber() const
	{
		return ItemNum;
	}

	virtual FText GetItemNumberText() const
	{
		return UKismetTextLibrary::Conv_IntToText(ItemNum);
	}

	virtual void SetItemNumber(const int NewNumber)
	{
		this->ItemNum = NewNumber > 0 ? NewNumber : 0;
		ItemChangedEvent.Broadcast(this);
	}

	virtual void UpdateItemNumber(const int DeltaNumber)
	{
		if ((ItemNum <= 0 && DeltaNumber < 0) || DeltaNumber == 0)
		{
			return ;
		}
		ItemNum = (ItemNum + DeltaNumber) > 0 ? ItemNum + DeltaNumber : 0;
		ItemChangedEvent.Broadcast(this);
		if (ItemNum == 0)
		{
			ItemExhaustEvent.Broadcast(this);
		}
	}

	virtual const FText& GetItemIntroduction() const
	{
		return ItemIntroduction;
	}

	virtual void SetItemIntroduction(const FText& NewIntroduction)
	{
		this->ItemIntroduction = NewIntroduction;
		ItemChangedEvent.Broadcast(this);
	}

	virtual const FText& GetItemDescription() const
	{
		return ItemDescription;
	}

	virtual void SetItemDescription(const FText& NewDescription)
	{
		this->ItemDescription = NewDescription;
		ItemChangedEvent.Broadcast(this);
	}

	virtual FItemChangeEvent& OnItemChanged()
	{
		return ItemChangedEvent;
	}

	virtual FItemChangeEvent& OnExhaust()
	{
		return ItemExhaustEvent;
	}

	virtual FItemBeSelectedEvent& OnItemBeSelected()
	{
		return ItemBeSelectedEvent;
	}

	virtual FItemBeSelectedEvent& OnItemDeselected()
	{
		return ItemDeselectedEvent;	
	}
};

UCLASS( ClassGroup=(ARPGBasic))
class UARPGCollectionComponent : public UARPGActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AARPGCollectableObject*> BagGameItems;

	int SelectedItemIndex = -1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameItemsManagerComponent",meta=(AllowPrivateAccess))
	bool RemoveGameItemFromBagAfterExhaust = false;

	DECLARE_MULTICAST_DELEGATE(FCollectionRefreshEvent);
	DECLARE_MULTICAST_DELEGATE_OneParam(FAddItemEvent, AARPGCollectableObject*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FRemoveItemEvent, AARPGCollectableObject*);

	FCollectionRefreshEvent CollectionRefreshEvent;
	
	FAddItemEvent AddItemToCollectionEvent;
	FRemoveItemEvent RemoveItemFromCollectionEvent;

public:
	UARPGCollectionComponent();

	UFUNCTION(BlueprintCallable,Category="ARPGCollectionComponent")
	bool AddItem(AARPGCollectableObject* NewGameItem);

	UFUNCTION(BlueprintCallable,Category="ARPGCollectionComponent")
	bool RemoveSelectedItem();

	UFUNCTION(BlueprintCallable,Category="ARPGCollectionComponent")
	bool SelectNextItem();

	UFUNCTION(BlueprintCallable,Category="ARPGCollectionComponent")
	bool SelectPreviousItem();

	UFUNCTION(BlueprintCallable,Category="ARPGCollectionComponent")
	TArray<AARPGCollectableObject*> GetAllItems() const
	{
		return BagGameItems;
	}

	template<typename T>
	TArray<T*> GetAllItems() const
	{
		TArray<T*> Result;
		for (auto GameItem : BagGameItems)
		{
			Result.Emplace(Cast<T>(GameItem));
		}
		return Result;
	}
	
	UFUNCTION(BlueprintCallable)
	int GetBagSelectedItemIndex() const { return this->SelectedItemIndex; }

	template<typename T>
	T* GetSelectedItem() const
	{
		return BagGameItems.IsValidIndex(SelectedItemIndex) ? Cast<T>(BagGameItems[SelectedItemIndex]) : nullptr;
	}



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:

	virtual FCollectionRefreshEvent& OnCollectionRefresh()
	{
		return CollectionRefreshEvent;
	}


	virtual FAddItemEvent& OnAddItemToCollection()
	{
		return AddItemToCollectionEvent;
	}

	virtual FRemoveItemEvent& OnRemoveItemFromCollection()
	{
		return RemoveItemFromCollectionEvent;
	}

};

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGBagComponent : public UARPGCollectionComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FCollectionItemUsedEvent, AARPGGameItem*);
	FCollectionItemUsedEvent ItemUsedEvent;

public:
	UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool TryToUseGameItemInBag(AARPGCharacter* User);


	virtual FCollectionItemUsedEvent& OnItemUsed()
	{
		return ItemUsedEvent;
	}
};

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UARPGSpellsManagerComponent : public UARPGCollectionComponent
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FSpellActivateEvent, AARPGCastAction*);
	FSpellActivateEvent SpellActivateEvent;

public:
	UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool TryToActivateCurrentSelectedSpell(AARPGCharacter* User = nullptr);
	
	virtual FSpellActivateEvent& OnSpellActivate()
	{
		return SpellActivateEvent;
	}
};