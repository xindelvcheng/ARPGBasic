// Fill out your copyright notice in the Description page of Project Settings.


#include "GameItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameItem.h"


bool UGameItemWidget::Initialize()
{
    Super::Initialize();
    if (!Image_Icon||!TextBlock_Number||!Image_BeSelectedIcon)
    {
        if (GEngine){GEngine->AddOnScreenDebugMessage(-1,5,FColor::Yellow,TEXT("GameItemWidget蓝图类控件与C++类不对应"));}
        
        return false;
    }

    Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Hidden);
    return true;
}

void UGameItemWidget::SetupGameItemWidget(AGameItem* NewGameItem)
{
    GameItem = NewGameItem;
    
    Image_Icon->SetBrushFromTexture(GameItem->ItemIcon);
    TextBlock_Number->SetText(FText::AsNumber(GameItem->Number));
    TextBlock_GameItemDisplayName->SetText(GameItem->GameItemDisplayName);

    GameItem->ItemWidget = this;
}

void UGameItemWidget::NativeSelectGameItemWidget()
{
    Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Visible);
    TextBlock_GameItemDisplayName->SetVisibility(ESlateVisibility::Visible);
    BeSelected();
}

void UGameItemWidget::NativeDeselectGameItemWidget()
{
    Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Hidden);
    TextBlock_GameItemDisplayName->SetVisibility(ESlateVisibility::Hidden);
    NotSelected();
}
