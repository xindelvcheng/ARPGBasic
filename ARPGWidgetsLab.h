// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"


#include "ARPGWidgetsLab.generated.h"

/**
* 
*/
UCLASS()
class TESTPROJECT_API UARPGWidgetsLab : public UUserWidget
{
    GENERATED_BODY()
};

UCLASS()
class UARPGNotifyWidget : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UImage* Image_Icon;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UTextBlock* TextBlock_Title;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UTextBlock* TextBlock_Content;

protected:
    virtual bool Initialize() override;
public:
    void ShowNotify(UTexture2D* Icon, FText Title, FText Content);
};


UCLASS()
class UARPGProgressBar : public UUserWidget
{
    GENERATED_BODY()

    float TotalValue;
    float TrueValue;
    float DummyValue;
    
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGProgressBar",meta=(BindWidget,AllowPrivateAccess))
    UProgressBar* ProgressBar;

protected:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UFUNCTION(BlueprintCallable,Category="ARPGProgressBar")
    void SetPercent(int Current,int Total);
    
};
