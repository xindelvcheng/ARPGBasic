// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"

#include "LoadGameWidget.generated.h"

UCLASS()
class  UArchiveItemWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    int ArchiveItemWidgetIndex;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(BindWidget))
    UButton* Button_Load;
};

/**
 * 
 */
UCLASS()
class  ULoadGameWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(BindWidget))
    UScrollBox* ScrollBox_Archives;
};


