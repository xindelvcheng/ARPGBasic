// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "ARPGGameItemsManagerComponent.h"
#include "ARPGWidgetsLab.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"


#include "ARPGStatusWidget.generated.h"

/**
 * 
 */

class AARPGGameItem;
enum class EBagEvent : unsigned char;
enum class ECharacterProperty : unsigned char;

UCLASS()
class UARPGStatusWidget : public UUserWidget
{
    GENERATED_BODY()

    FTimerHandle ClearNotifiesTimerHandle;
    FTimerDelegate ClearNotifiesTimerDelegate;

    UPROPERTY()
    UARPGGameItemsManagerComponent* ItemsManagerComponent;

    int BagItemsNum;

protected:
    virtual bool Initialize() override;


    virtual void BeginDestroy() override;

    TSubclassOf<class UARPGNotifyWidget> NotifyWidgetClass;
    TSubclassOf<class UGameItemWidget> GameItemWidgetClass;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    class UARPGProgressBar* SmoothProgressBar_HP;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    class UARPGProgressBar* SmoothProgressBar_SP;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UTextBlock* TextBlock_Coins;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UScrollBox* ScrollBox_Notifications;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UUniformGridPanel* UniformGridPanel_Bag;

    UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
    UTextBlock* TextBlock_Dialog;

    UFUNCTION(Category="ARPGStatusWidget")
    void UpdateBagWidget(EBagEvent BagEvent, AARPGGameItem* GameItem);

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
