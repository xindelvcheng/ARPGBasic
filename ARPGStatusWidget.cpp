// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStatusWidget.h"

#include "ARPGBasicSettings.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ARPGWidgetsLab.h"

#include "ARPGGameInstanceSubsystem.h"
#include "ARPGWidgetsLab.h"
#include "Components/ScrollBoxSlot.h"
#include "ARPGGameItemsManagerComponent.h"
#include "Kismet/KismetTextLibrary.h"

bool UARPGStatusWidget::Initialize()
{
    Super::Initialize();

    if (!ScrollBox_Notifications || !UniformGridPanel_Bag)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, TEXT("UARPGStatusWidget的蓝图类控件与C++中指定的不一致"));
        }
        return false;
    }
    if (UARPGBasicSettings::Get())
    {
        NotifyWidgetClass = LoadClass<UARPGNotifyWidget>(
            nullptr, *UARPGBasicSettings::Get()->NotifyWidgetClass.ToSoftObjectPath().ToString());

        verifyf(NotifyWidgetClass, TEXT("没有设定NotifyWidgetClass、HPBarClass、HPBarClass"))

        GameItemWidgetClass = LoadClass<UGameItemWidget>(
            nullptr, *UARPGBasicSettings::Get()->GameItemWidgetClass.ToSoftObjectPath().ToString());

        verifyf(GameItemWidgetClass, TEXT("没有设定GameItemWidgetClass"));
    }

    ScrollBox_Notifications->SetRenderOpacity(0);
    return true;
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

    //根据主角的当前属性进行初始化
    SmoothProgressBar_HP->SetPercent(MainCharacter->GetCurrentHP(), MainCharacter->GetMaxHP());
    SmoothProgressBar_SP->SetPercent(MainCharacter->GetCurrentSP(), MainCharacter->GetMaxSP());
    TextBlock_Coins->SetText(UKismetTextLibrary::Conv_IntToText(MainCharacter->GetCoins()));

    //绑定主角的属性更改事件
    MainCharacter->OnCharacterPropertyChanged().AddDynamic(this, &UARPGStatusWidget::ProcessCharacterPropertiesChanged);


    //清空背包栏
    UniformGridPanel_Bag->ClearChildren();
    BagItemsNum = 0;
    for (int i = 0; i < 9; ++i)
    {
        UGameItemWidget* GameItemWidget = Cast<UGameItemWidget>(CreateWidget(GetOwningPlayer(), GameItemWidgetClass));
        UniformGridPanel_Bag->AddChildToUniformGrid(GameItemWidget, i / 3, i % 3);
        GameItemWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    ItemsManagerComponent = MainCharacter->GetGameItemsManagerComponent();
    //绑定背包事件
    if (ItemsManagerComponent)
    {
        ItemsManagerComponent->OnBagChanged.AddDynamic(this, &UARPGStatusWidget::UpdateBagWidget);
    }
}

void UARPGStatusWidget::UpdateBagWidget(EBagEvent BagEvent, AGameItem* GameItem)
{
    if (!GameItem && BagEvent != EBagEvent::ReFlush)
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("背包更新的GameItem为空"));
        return;
    }
    switch (BagEvent)
    {
    case EBagEvent::ReFlush:
        BagItemsNum = 0;
        if (ItemsManagerComponent)
        {
            for (AGameItem* GameItemsInBag : ItemsManagerComponent->GetAllGameItemsInBag())
            {
                UGameItemWidget* GameItemWidget = Cast<UGameItemWidget>(UniformGridPanel_Bag->GetChildAt(BagItemsNum));
                GameItemWidget->SetupGameItemWidget(GameItemsInBag);
                GameItemWidget->SetVisibility(ESlateVisibility::Visible);
                BagItemsNum++;
            }
        }
        for (int j = BagItemsNum; j < 9; ++j)
        {
            UGameItemWidget* GameItemWidget = Cast<UGameItemWidget>(UniformGridPanel_Bag->GetChildAt(j));
            GameItemWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        break;
    case EBagEvent::AddNewItemToBag:
        {
            UGameItemWidget* GameItemWidget = Cast<UGameItemWidget>(UniformGridPanel_Bag->GetChildAt(BagItemsNum));
            GameItemWidget->SetupGameItemWidget(GameItem);
            GameItemWidget->SetVisibility(ESlateVisibility::Visible);
            BagItemsNum++;
        }
        break;
    case EBagEvent::ChangeItemNumbers:
        if (GameItem->ItemWidget)
        {
            GameItem->ItemWidget->SetupGameItemWidget(GameItem);
        }
        break;
    case EBagEvent::SelectItemInBag:
        if (GameItem->ItemWidget)
        {
            GameItem->ItemWidget->BeSelected();
        }
        break;
    case EBagEvent::DeselectItemInBag:
        if (GameItem->ItemWidget)
        {
            GameItem->ItemWidget->NotSelected();
        }
        break;
    case EBagEvent::UseItemInBag:
        if (GameItem->ItemWidget)
        {
            GameItem->ItemWidget->SetupGameItemWidget(GameItem);
        }
        break;
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
}
