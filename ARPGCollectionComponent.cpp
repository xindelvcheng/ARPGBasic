// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCollectionComponent.h"

#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGGameItem.h"
#include "CollectableItemWidget.h"
#include "ARPGCastAction.h"

AARPGCollectableObject::AARPGCollectableObject()
{
	if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
	{
		if (!ItemIcon)
		{
			ItemIcon = BasicSettings->DefaultGameItemIcon.LoadSynchronous();
		}
	}
}

// Sets default values for this component's properties
UARPGCollectionComponent::UARPGCollectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UARPGCollectionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CollectionRefreshEvent.Broadcast();
}


// Called every frame
void UARPGCollectionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UARPGCollectionComponent::AddItem(AARPGCollectableObject* NewGameItem)
{
	if (!NewGameItem)
	{
		return false;
	}
	for (int i = 0; i < BagGameItems.Num(); ++i)
	{
		AARPGCollectableObject* Item = BagGameItems[i];
		if (!Item)
		{
			BagGameItems.Remove(Item);
			continue;
		}

		if (Item->GetItemName() == NewGameItem->GetItemName())
		{
			Item->UpdateItemNumber(NewGameItem->GetItemNumber());

			return true;
		}
	}

	BagGameItems.Emplace(NewGameItem);
	AddItemToCollectionEvent.Broadcast(NewGameItem);
	NewGameItem->OnExhaust().AddLambda([&](AARPGCollectableObject* Item)
	{
		if (RemoveGameItemFromBagAfterExhaust)
		{
			BagGameItems.Remove(Item);
			CollectionRefreshEvent.Broadcast();
		}
	});

	if (SelectedItemIndex < 0)
	{
		SelectNextItem();
	}

	return true;
}

bool UARPGCollectionComponent::RemoveSelectedItem()
{
	if (!BagGameItems.IsValidIndex(SelectedItemIndex))
	{
		return false;
	}
	AARPGCollectableObject* GameItem = BagGameItems[SelectedItemIndex];
	BagGameItems.RemoveAt(SelectedItemIndex);
	RemoveItemFromCollectionEvent.Broadcast(GameItem);
	return true;
}

bool UARPGCollectionComponent::SelectNextItem()
{
	if (BagGameItems.Num() <= 0)
	{
		return false;
	}

	if (BagGameItems.IsValidIndex(SelectedItemIndex))
	{
		const auto PreviousItem = BagGameItems[SelectedItemIndex];
		PreviousItem->DeselectItem();
	}

	SelectedItemIndex = (SelectedItemIndex + 1) % BagGameItems.Num();
	if (BagGameItems.IsValidIndex(SelectedItemIndex))
	{
		const auto CurrentItem = BagGameItems[SelectedItemIndex];
		CurrentItem->SelectItem();
	}
	return true;
}

bool UARPGCollectionComponent::SelectPreviousItem()
{
	if (BagGameItems.Num() <= 0)
	{
		return false;
	}

	if (BagGameItems.IsValidIndex(SelectedItemIndex))
	{
		const auto PreviousItem = BagGameItems[SelectedItemIndex];
		PreviousItem->DeselectItem();
	}

	SelectedItemIndex = (SelectedItemIndex + BagGameItems.Num() - 1) % BagGameItems.Num();
	if (BagGameItems.IsValidIndex(SelectedItemIndex))
	{
		const auto CurrentItem = BagGameItems[SelectedItemIndex];
		CurrentItem->SelectItem();
	}
	return true;
}

bool UARPGBagComponent::TryToUseGameItemInBag(AARPGCharacter* User)
{
	if (AARPGGameItem* GameItem = GetSelectedItem<AARPGGameItem>())
	{
		if (GameItem->TryToUseGameItem(User))
		{
			GameItem->UpdateItemNumber(-1);

			ItemUsedEvent.Broadcast(GameItem);
			return true;
		}
	};

	return false;
}

bool UARPGSpellsManagerComponent::TryToActivateCurrentSelectedSpell(AARPGCharacter* User)
{
	User = User ? User : GetOwnerCharacter();
	if (!User)
	{
		return false;	
	}
	
	if (AARPGCastAction* Spell = GetSelectedItem<AARPGCastAction>())
	{
		return User->TryToUseAbility(Spell);
	};

	return false;
}
