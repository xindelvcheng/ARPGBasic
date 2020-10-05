// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

class UMessageWidget;

UCLASS()
class  UARPGStatusWidget   : public UUserWidget
{
	GENERATED_BODY()

	FTimerHandle ClearNotifiesTimerHandle;
	FTimerDelegate ClearNotifiesTimerDelegate;

protected:
	virtual bool Initialize() override;


	virtual void BeginDestroy() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ARPGStatusWidget")
	TSubclassOf<class UARPGNotifyWidget> NotifyWidgetClass;
	
	UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
	UScrollBox* ScrollBox_Notifications;

	UPROPERTY(BlueprintReadOnly,Category="ARPGStatusWidget",meta=(BindWidget))
	UUniformGridPanel* UniformGridPanel_Bag;
	

	UFUNCTION(BlueprintImplementableEvent,Category="ARPGStatusWidget")
	void UpdateBagWidget();

public:
	
	UFUNCTION(BlueprintCallable)
	void ShowNotify(UTexture2D* Icon,FText Title,FText Content);
};
