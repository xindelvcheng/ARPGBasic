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

void UGameItemWidget::SetupGameItemWidget(AGameItem* GameItem)
{
    Image_Icon->SetBrushFromTexture(GameItem->ItemIcon);
    TextBlock_Number->SetText(FText::AsNumber(GameItem->Number));

    GameItem->ItemWidget = this;
}
