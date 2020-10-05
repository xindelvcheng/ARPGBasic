// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"



#include "PromptWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class  UPromptWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGBasic",meta=(BindWidget))
    UButton* Button_Yes;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGBasic",meta=(BindWidget))
    UButton* Button_No;
};

