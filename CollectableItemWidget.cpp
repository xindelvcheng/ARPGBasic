// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ARPGGameItem.h"


bool UCollectableItemWidget::Initialize()
{
	Super::Initialize();
	if (!Image_ItemIcon || !TextBlock_ItemNum || !Image_BeSelectedIcon)
	{
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("GameItemWidget蓝图类控件与C++类不对应")); }

		return false;
	}

	Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Hidden);
	return true;
}

void UCollectableItemWidget::OnWidgetBeSelected()
{
	Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Visible);
	TextBlock_ItemDisplayName->SetVisibility(ESlateVisibility::Visible);
}

void UCollectableItemWidget::OnWidgetDeselected()
{
	Image_BeSelectedIcon->SetVisibility(ESlateVisibility::Hidden);
	TextBlock_ItemDisplayName->SetVisibility(ESlateVisibility::Hidden);
}

void UCollectableItemWidget::BindToItem(AARPGCollectableObject* NewGameItem)
{
	ClearItemWidgetAndUnbindDelegates();
	
	Item = NewGameItem;
	Image_ItemIcon->SetBrushFromTexture(NewGameItem->GetItemIcon());
	TextBlock_ItemNum->SetText(NewGameItem->GetItemNumberText());
	TextBlock_ItemDisplayName->SetText(NewGameItem->GetGameItemDisplayName());
	SetVisibility(ESlateVisibility::Visible);

	DelegateHandles.Emplace(Item->OnItemChanged().AddLambda([&](AARPGCollectableObject*)
	{
		Image_ItemIcon->SetBrushFromTexture(Item->GetItemIcon());
		TextBlock_ItemNum->SetText(FText::AsNumber(Item->GetItemNumber()));
		TextBlock_ItemDisplayName->SetText(Item->GetGameItemDisplayName());
	}));

	DelegateHandles.Emplace(Item->OnItemBeSelected().AddLambda([&](AARPGCollectableObject*)
	{
		OnWidgetBeSelected();
		BPFunc_OnWidgetBeSelected();
	}));

	DelegateHandles.Emplace(Item->OnItemDeselected().AddLambda([&](AARPGCollectableObject*)
	{
		OnWidgetDeselected();
		BPFunc_OnWidgetDeselected();
	}));
}

void UCollectableItemWidget::ClearItemWidgetAndUnbindDelegates()
{
	
	if (Item.Get())
	{
		for (const FDelegateHandle DelegateHandle : DelegateHandles)
		{
			Item->OnItemChanged().Remove(DelegateHandle);
		}
		DelegateHandles.Empty();
	}
	
	Item = nullptr;
	Image_ItemIcon->SetBrushFromTexture(nullptr);
	TextBlock_ItemNum->SetText(FText{});
	TextBlock_ItemDisplayName->SetText(FText{});

	SetVisibility(ESlateVisibility::Hidden);
}
