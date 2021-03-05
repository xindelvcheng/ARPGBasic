// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


#include "CollectableItemWidget.generated.h"

class AARPGCollectableObject;
class UImage;
class UTextBlock;

/**
 * 用于显示物品的小窗体，若未绑定物体，其是不可见的
 */
UCLASS()
class  UCollectableItemWidget : public UUserWidget
{
	GENERATED_BODY()

	TArray<FDelegateHandle> DelegateHandles;

protected:
	virtual bool Initialize() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UImage* Image_ItemIcon;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UImage* Image_BeSelectedIcon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UTextBlock* TextBlock_ItemNum;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UTextBlock* TextBlock_ItemDisplayName;

	UPROPERTY(BlueprintReadOnly,Category="GameItemWidget")
	TWeakObjectPtr<AARPGCollectableObject> Item;
	
	virtual void OnWidgetBeSelected();

	virtual void OnWidgetDeselected();

public:
	UFUNCTION(BlueprintCallable)
	void BindToItem(AARPGCollectableObject* NewGameItem);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnWidgetBeSelected",Category = "GameItemWidget")
	void BPFunc_OnWidgetBeSelected();

	UFUNCTION(BlueprintImplementableEvent,DisplayName="OnWidgetDeselected", Category = "GameItemWidget")
	void BPFunc_OnWidgetDeselected();
	

	void ClearItemWidgetAndUnbindDelegates();
};
