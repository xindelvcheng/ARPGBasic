// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameItem.h"
#include "Blueprint/UserWidget.h"


#include "GameItemWidget.generated.h"

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
	class UImage* Image_Icon;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	class UImage* Image_BeSelectedIcon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="GameItemWidget",meta=(BindWidget))
	class UTextBlock* Text_Number;

	UPROPERTY(EditDefaultsOnly,Category="GameItemWidget")
	class UTexture2D* BeSelectedIcon;

public:
	UFUNCTION(BlueprintCallable)
	void SetupGameItemWidget(class AGameItem* GameItem);

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, Category = "GameItemWidget")
	void BeSelected();

	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, Category = "GameItemWidget")
	void NotSelected();
};
