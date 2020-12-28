// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARPGGameItemsManagerComponent.generated.h"


class AARPGCharacter;
class AARPGGameItem;

UENUM(BlueprintType)
enum class EBagEventType:uint8
{
    AddNewItemToBag,
    ChangeItemNumbers,
    SelectItemInBag,
    DeselectItemInBag,
    UseItemInBag,
    ReFlush
};

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class  UARPGGameItemsManagerComponent : public UActorComponent
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<class AARPGGameItem*> Bag;
    TArray<class AGameItems*> Repository;

    int BagSelectedItemIndex = -1;


    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBagChangedEvent,EBagEventType,BagEventType ,AARPGGameItem*, GameItem);

public:
    UPROPERTY(BlueprintAssignable)
    FBagChangedEvent OnBagChanged;

    UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool AddItemToBag(AARPGGameItem* GameItem);

    UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool SelectNextItemInBag();

    UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool SelectPreviousItemInBag();

    UFUNCTION(BlueprintCallable,Category="BagSystem")
    bool UseItemInBag(AARPGCharacter* User);

    UFUNCTION(BlueprintCallable,Category="BagSystem")
    TArray<AARPGGameItem*> GetAllGameItemsInBag() const { return this->Bag; }

    void SetBag(TArray<AARPGGameItem*> bag) { this->Bag = bag; OnBagChanged.Broadcast(EBagEventType::ReFlush,nullptr);}

    UFUNCTION(BlueprintCallable)
    int GetBagSelectedItemIndex() const { return this->BagSelectedItemIndex; }

public:
    // Sets default values for this component's properties
    UARPGGameItemsManagerComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
