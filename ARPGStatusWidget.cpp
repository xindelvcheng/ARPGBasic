// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStatusWidget.h"

#include "ARPGBasicSettings.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ARPGWidgetsLab.h"

#include "ARPGGameInstanceSubsystem.h"
#include "ARPGWidgetsLab.h"
#include "Components/ScrollBoxSlot.h"
#include "ARPGCollectionComponent.h"
#include "ARPGMainCharacter.h"
#include "Kismet/KismetTextLibrary.h"


bool UARPGStatusWidget::Initialize()
{
	Super::Initialize();

	if (!ScrollBox_Notifications || !HorizontalBox_GameItems || !HorizontalBox_GameItems)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("UARPGStatusWidget的蓝图类控件与C++中指定的不一致"));
		}
		return false;
	}

	if (UARPGBasicSettings::Get())
	{
		if (!NotifyWidgetClass)
		{
			NotifyWidgetClass = UARPGBasicSettings::Get()->NotifyWidgetClass.LoadSynchronous();
		}
		if (!ItemWidgetClass)
		{
			ItemWidgetClass = UARPGBasicSettings::Get()->GameItemWidgetClass.LoadSynchronous();
		}
	}


	ScrollBox_Notifications->SetRenderOpacity(0);

	TextBlock_Dialog->SetAutoWrapText(true);
	TextBlock_Dialog->SetJustification(ETextJustify::Center);

	return true;
}

void UARPGStatusWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	
}

void UARPGStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UARPGStatusWidget::BeginDestroy()
{
	Super::BeginDestroy();
}

void UARPGStatusWidget::ShowNotify(UTexture2D* Icon, FText Title, FText Content)
{
	UARPGNotifyWidget* NotifyWidget = Cast<UARPGNotifyWidget>(
		CreateWidget(UARPGGameInstanceSubsystem::GetMainCharacterController(GetWorld()), NotifyWidgetClass));

	verifyf(NotifyWidget, TEXT("未指定NotifyWidgetClass"))

	NotifyWidget->ShowNotify(Icon, Title, Content);
	TArray<UWidget*> Widgets = ScrollBox_Notifications->GetAllChildren();
	ScrollBox_Notifications->ClearChildren();
	ScrollBox_Notifications->AddChild(NotifyWidget);
	for (auto Widget : Widgets)
	{
		ScrollBox_Notifications->AddChild(Widget);
	}
	ScrollBox_Notifications->SetRenderOpacity(1);

	GetWorld()->GetTimerManager().ClearTimer(ClearNotifiesTimerHandle);

	if (!ClearNotifiesTimerDelegate.IsBound())
	{
		ClearNotifiesTimerDelegate.BindLambda([&]()
		{
			ScrollBox_Notifications->ClearChildren();
			ScrollBox_Notifications->SetRenderOpacity(0);
		});
	}

	GetWorld()->GetTimerManager().SetTimer(ClearNotifiesTimerHandle, ClearNotifiesTimerDelegate, 5, false);
}

void UARPGStatusWidget::BindToMainCharacter(AARPGMainCharacter* MainCharacter)
{
	verify(SmoothProgressBar_HP&&SmoothProgressBar_SP&&TextBlock_Coins);

	//初始化背包栏
	HorizontalBox_GameItems->ClearChildren();
	BagItemsNum = 0;
	for (int i = 0; i < BagBlocksNum; ++i)
	{
		UCollectableItemWidget* ItemWidget = CreateWidget<UCollectableItemWidget>(
            GetOwningPlayer(), ItemWidgetClass);
		HorizontalBox_GameItems->AddChildToHorizontalBox(ItemWidget);
	}

	//初始化技能栏
	UniformGridPanel_Spells->ClearChildren();
	SpellsNum = 0;

	for (int i = 0; i < SpellGridRowsNum * SpellGridColumnsNum; ++i)
	{
		UCollectableItemWidget* ItemWidget = CreateWidget<UCollectableItemWidget>(
            GetOwningPlayer(), ItemWidgetClass);
		UniformGridPanel_Spells->AddChildToUniformGrid(ItemWidget, i / SpellGridRowsNum, i % SpellGridColumnsNum);
	}

	//根据主角的当前属性进行初始化
	SmoothProgressBar_HP->SetPercent(MainCharacter->GetCurrentHP(), MainCharacter->GetMaxHP());
	SmoothProgressBar_SP->SetPercent(MainCharacter->GetCurrentSP(), MainCharacter->GetMaxSP());
	TextBlock_Coins->SetText(UKismetTextLibrary::Conv_IntToText(MainCharacter->GetCoins()));

	//绑定主角的属性更改事件
	MainCharacter->OnCharacterPropertyChanged().AddDynamic(this, &UARPGStatusWidget::ProcessCharacterPropertiesChanged);

	ItemsManagerComponent = MainCharacter->GetGameItemsManagerComponent();
	//绑定背包事件
	if (ItemsManagerComponent.IsValid())
	{
		ItemsManagerComponent->OnCollectionRefresh().AddLambda([&]()
		{
			GameItemUIMap.Empty();
			BagItemsNum = 0;
			if (ItemsManagerComponent.IsValid())
			{
				for (AARPGGameItem* GameItemInBag : ItemsManagerComponent->GetAllItems<AARPGGameItem>())
				{
					UCollectableItemWidget* ItemWidget = Cast<UCollectableItemWidget>(
						HorizontalBox_GameItems->GetChildAt(BagItemsNum));
					ItemWidget->BindToItem(GameItemInBag);
					GameItemUIMap.Add(GameItemInBag, ItemWidget);
					BagItemsNum++;
				}
			}
			for (int j = BagItemsNum; j < BagBlocksNum; ++j)
			{
				UCollectableItemWidget* ItemWidget = Cast<UCollectableItemWidget>(
					HorizontalBox_GameItems->GetChildAt(j));
				ItemWidget->ClearItemWidgetAndUnbindDelegates();
			}
		});

		ItemsManagerComponent->OnAddItemToCollection().AddLambda([&](AARPGCollectableObject* Item)
		{
			UCollectableItemWidget* GameItemWidget = Cast<UCollectableItemWidget>(
				HorizontalBox_GameItems->GetChildAt(BagItemsNum));
			AARPGGameItem* GameItem = Cast<AARPGGameItem>(Item);
			GameItemWidget->BindToItem(GameItem);
			GameItemUIMap.Add(GameItem, GameItemWidget);
			BagItemsNum++;
		});

		ItemsManagerComponent->OnRemoveItemFromCollection().AddLambda([&](AARPGCollectableObject* Item)
		{
			AARPGGameItem* GameItem = Cast<AARPGGameItem>(Item);
			UCollectableItemWidget* GameItemWidget = GameItemUIMap.FindRef(GameItem);
			GameItemWidget->ClearItemWidgetAndUnbindDelegates();
			GameItemUIMap.Remove(GameItem);
		});
	}
	SpellsManagerComponent = MainCharacter->GetCharacterSpellsManagerComponent();
	if (SpellsManagerComponent.IsValid())
	{
		SpellsManagerComponent->OnCollectionRefresh().AddLambda([&]()
		{
			SpellsNum = 0;
			SpellUIMap.Empty();
			if (SpellsManagerComponent.IsValid())
			{
				for (AARPGCastAction* Spell : SpellsManagerComponent->GetAllItems<AARPGCastAction>())
				{
					UCollectableItemWidget* ItemWidget = Cast<UCollectableItemWidget>(
						UniformGridPanel_Spells->GetChildAt(SpellsNum));
					ItemWidget->BindToItem(Spell);
					SpellUIMap.Add(Spell, ItemWidget);
					SpellsNum++;
				}
			}
			for (int j = SpellsNum; j < SpellGridRowsNum * SpellGridColumnsNum; ++j)
			{
				UCollectableItemWidget* ItemWidget = Cast<UCollectableItemWidget>(
					UniformGridPanel_Spells->GetChildAt(j));
				ItemWidget->ClearItemWidgetAndUnbindDelegates();
			}
		});

		SpellsManagerComponent->OnAddItemToCollection().AddLambda([&](AARPGCollectableObject* Item)
		{
			UCollectableItemWidget* SpellWidget = Cast<UCollectableItemWidget>(
				UniformGridPanel_Spells->GetChildAt(SpellsNum));
			SpellWidget->BindToItem(Item);
			SpellUIMap.Add(Item, SpellWidget);
			SpellsNum++;
		});

		/*TODO：如果将法术改为可按位置随意摆放，此处取消注释*/
		// SpellsManagerComponent->OnRemoveItemFromCollection().AddLambda([&](AARPGCollectableObject* Item)
		// {
		// 	AARPGGameItem* Spell = Cast<AARPGGameItem>(Item);
		// 	UCollectableItemWidget* GameItemWidget = SpellUIMap.FindRef(Spell);
		// 	GameItemWidget->ClearItemWidgetAndUnbindDelegates();
		// 	SpellUIMap.Remove(Spell);
		// });
	}
}


void UARPGStatusWidget::ProcessCharacterPropertiesChanged(ECharacterProperty ChangedProperty, int CurrentValue,
                                                          int TotalValue, int DeltaValue)
{
	switch (ChangedProperty)
	{
	case ECharacterProperty::CurrentHP:
		SmoothProgressBar_HP->SetPercent(CurrentValue, TotalValue);
		break;
	case ECharacterProperty::CurrentSP:
		SmoothProgressBar_SP->SetPercent(CurrentValue, TotalValue);
		break;
	case ECharacterProperty::Coins:
		TextBlock_Coins->SetText(UKismetTextLibrary::Conv_IntToText(CurrentValue));
		break;
	default:
		break;
	}
	BPFunc_OnCharacterPropertiesChanged(ChangedProperty, CurrentValue, TotalValue, DeltaValue);
}
