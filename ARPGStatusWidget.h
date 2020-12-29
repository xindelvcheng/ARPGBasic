// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "ARPGCastAction.h"
#include "ARPGWidgetsLab.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"


#include "ARPGStatusWidget.generated.h"

class UARPGCollectionComponent;

/**
 * 
 */


class UCollectableItemWidget;
class AARPGGameItem;
enum class ECharacterProperty : unsigned char;

UCLASS()
class UARPGStatusWidget : public UUserWidget
{
    GENERATED_BODY()

    FTimerHandle ClearNotifiesTimerHandle;
    FTimerDelegate ClearNotifiesTimerDelegate;

    
    TWeakObjectPtr<UARPGBagComponent> ItemsManagerComponent;
    TWeakObjectPtr<UARPGSpellsManagerComponent> SpellsManagerComponent;

    int BagItemsNum;
    int BagBlocksNum = 3;
    
    int SpellsNum;
    int SpellGridRowsNum = 3;
    int SpellGridColumnsNum = 3;

    UPROPERTY()
    TMap<AARPGCollectableObject*,UCollectableItemWidget*> GameItemUIMap;
    UPROPERTY()
    TMap<AARPGCollectableObject*,UCollectableItemWidget*> SpellUIMap;
    

protected:
    virtual bool Initialize() override;

    virtual void NativePreConstruct() override;

    virtual void NativeConstruct() override;

    virtual void BeginDestroy() override;

    TSubclassOf<class UARPGNotifyWidget> NotifyWidgetClass;
    TSubclassOf<class UCollectableItemWidget> ItemWidgetClass;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    class UARPGProgressBar* SmoothProgressBar_HP;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    class UARPGProgressBar* SmoothProgressBar_SP;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UTextBlock* TextBlock_Coins;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UScrollBox* ScrollBox_Notifications;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UUniformGridPanel* UniformGridPanel_Spells;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UHorizontalBox* HorizontalBox_GameItems;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UTextBlock* TextBlock_Dialog;

public:
    
    UFUNCTION(BlueprintCallable)
    void ShowNotify(UTexture2D* Icon, FText Title, FText Content);

    UFUNCTION(BlueprintCallable)
    void BindToMainCharacter(class AARPGMainCharacter* MainCharacter);

    UFUNCTION()
    void ProcessCharacterPropertiesChanged(ECharacterProperty hangedProperty, int CurrentValue, int TotalValue,
                                           int DeltaValue);


    UFUNCTION(BlueprintImplementableEvent,Category="ARPGStatusWidget",DisplayName="OnCharacterPropertiesChanged")
    void BPFunc_OnCharacterPropertiesChanged(ECharacterProperty hangedProperty, int CurrentValue, int TotalValue,
                                             int DeltaValue);
};
