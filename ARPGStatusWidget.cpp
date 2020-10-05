// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGStatusWidget.h"

#include "ARPGBasicSettings.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "ARPGGameInstanceSubsystem.h"
#include "ARPGWidgetsLab.h"
#include "Components/ScrollBoxSlot.h"

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
        FString String = UARPGBasicSettings::Get()->NotifyWidgetClass.ToString();
        NotifyWidgetClass = LoadClass<UARPGNotifyWidget>(
            nullptr, *UARPGBasicSettings::Get()->NotifyWidgetClass.ToSoftObjectPath().ToString());
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
    UARPGNotifyWidget* NotifyWidget = Cast<UARPGNotifyWidget>(CreateWidget(GetOwningPlayer(), NotifyWidgetClass));
    
    verifyf(NotifyWidget,TEXT("未指定NotifyWidgetClass"))
    
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
