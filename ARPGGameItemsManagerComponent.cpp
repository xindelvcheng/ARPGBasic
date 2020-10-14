// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameItemsManagerComponent.h"
#include "ARPGCharacter.h"
#include "GameItem.h"
#include "GameItemWidget.h"

// Sets default values for this component's properties
UARPGGameItemsManagerComponent::UARPGGameItemsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UARPGGameItemsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
    OnBagChanged.Broadcast(EBagEvent::ReFlush,nullptr);
}


// Called every frame
void UARPGGameItemsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UARPGGameItemsManagerComponent::AddItemToBag(AGameItem* GameItem){
    if (!GameItem)
    {
        return false;
    }
    for (int i = 0; i < Bag.Num(); ++i)
    {
        AGameItem* Item = Bag[i];
        if (!Item)
        {
            Bag.Remove(Item);
            continue;
        }

        if (Item->ItemName == GameItem->ItemName)
        {
            Item->Number += GameItem->Number;
            OnBagChanged.Broadcast(EBagEvent::ChangeItemNumbers,Item);
            return true;
        }
    }
    Bag.Emplace(GameItem);
    OnBagChanged.Broadcast(EBagEvent::AddNewItemToBag,GameItem);

    if (BagSelectedItemIndex < 0)
    {
        SelectNextItemInBag();
    }
    
    return true;
}

bool UARPGGameItemsManagerComponent::SelectNextItemInBag()
{
    if (Bag.Num() <= 0)
    {
        return false;
    }

    if (Bag.IsValidIndex(BagSelectedItemIndex))
    {
        const auto PreviousItem = Bag[BagSelectedItemIndex];
        OnBagChanged.Broadcast(EBagEvent::DeselectItemInBag,PreviousItem);
    }

    BagSelectedItemIndex = (BagSelectedItemIndex + 1) % Bag.Num();
    if (Bag.IsValidIndex(BagSelectedItemIndex))
    {
        const auto CurrentItem = Bag[BagSelectedItemIndex];
        OnBagChanged.Broadcast(EBagEvent::SelectItemInBag,CurrentItem);
    }
    return true;
}

bool UARPGGameItemsManagerComponent::SelectPreviousItemInBag()
{
    if (Bag.Num() <= 0)
    {
        return false;
    }

    if (Bag.IsValidIndex(BagSelectedItemIndex))
    {
        const auto PreviousItem = Bag[BagSelectedItemIndex];
        OnBagChanged.Broadcast(EBagEvent::DeselectItemInBag,PreviousItem);
    }

    BagSelectedItemIndex = (BagSelectedItemIndex + Bag.Num() - 1) % Bag.Num();
    if (Bag.IsValidIndex(BagSelectedItemIndex))
    {
        const auto CurrentItem = Bag[BagSelectedItemIndex];
        OnBagChanged.Broadcast(EBagEvent::SelectItemInBag,CurrentItem);
    }
    return true;
}

bool UARPGGameItemsManagerComponent::UseItemInBag(AARPGCharacter* User)
{
    if (!Bag.IsValidIndex(BagSelectedItemIndex))
    {
        return false;
    }

    AGameItem* GameItem = Bag[BagSelectedItemIndex];
    GameItem->NativeUseGameItem(User);
    GameItem->Number--;
    if (GameItem->Number > 0)
    {
        OnBagChanged.Broadcast(EBagEvent::UseItemInBag,GameItem);
    }else
    {
        Bag.RemoveAt(BagSelectedItemIndex);
        OnBagChanged.Broadcast(EBagEvent::ReFlush,nullptr);
    }
    return true;
}

