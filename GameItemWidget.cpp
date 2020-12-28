// Fill out your copyright notice in the Description page of Project Settings.


#include "GameItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ARPGGameItem.h"


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

void UGameItemWidget::SetupGameItemWidget(AARPGGameItem* NewGameItem)
{
    GameItem = NewGameItem;
    
    Image_Icon->SetBrushFromTexture(GameItem->GetItemIcon());
    TextBlock_Number->SetText(FText::AsNumber(GameItem->GetNumber()));
    TextBlock_GameItemDisplayName->SetText(GameItem->GetGameItemDisplayName());

    GameItem->SetItemWidget(this);
}

void UGameItemWidget::SelectGameItemWidget()
{
    Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Visible);
    TextBlock_GameItemDisplayName->SetVisibility(ESlateVisibility::Visible);
    BPFunc_OnWidgetBeSelected();
}

void UGameItemWidget::DeselectGameItemWidget()
{
    Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Hidden);
    TextBlock_GameItemDisplayName->SetVisibility(ESlateVisibility::Hidden);
    BPFunc_OnWidgetDeselected();
}
