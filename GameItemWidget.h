// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARPGGameItem.h"
#include "Blueprint/UserWidget.h"


#include "GameItemWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class  UGameItemWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UImage* Image_Icon;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UImage* Image_BeSelectedIcon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UTextBlock* TextBlock_Number;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	UTextBlock* TextBlock_GameItemDisplayName;

	UPROPERTY(BlueprintReadOnly,Category="GameItemWidget")
	TWeakObjectPtr<AARPGGameItem> GameItem;

public:
	UFUNCTION(BlueprintCallable)
	void SetupGameItemWidget(AARPGGameItem* NewGameItem);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameItemWidget")
	void BPFunc_OnWidgetBeSelected();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameItemWidget")
	void BPFunc_OnWidgetDeselected();
	
	UFUNCTION(BlueprintCallable,DisplayName="SelectGameItemWidget", Category = "GameItemWidget")
	void SelectGameItemWidget();

	UFUNCTION(BlueprintCallable,DisplayName="DeselectGameItemWidget", Category = "GameItemWidget")
	void DeselectGameItemWidget();
	
};

typedef UGameItemWidget USpellWidget;
