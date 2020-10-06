// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGWidgetsLab.h"


#include "ARPGGameInstanceSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"


bool UARPGNotifyWidget::Initialize()
{
    Super::Initialize();

    TextBlock_Title->SetAutoWrapText(true);
    TextBlock_Content->SetAutoWrapText(true);
    return true;
}

void UARPGNotifyWidget::ShowNotify(UTexture2D* Icon, FText Title, FText Content)
{
    Image_Icon->SetBrushFromTexture(Icon);
    TextBlock_Title->SetText(Title);
    TextBlock_Content->SetText(Content);
}


bool UARPGProgressBar::Initialize()
{
    Super::Initialize();

    return true;
}

void UARPGProgressBar::SetPercent(int Current, int Total)
{
    TrueValue = static_cast<float>(Current);
    TotalValue = static_cast<float>(Total);

    int Length = static_cast<int>(300*(1 + TotalValue/((TotalValue/3)+300)));
    if (GEngine)
    {
        UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ProgressBar->Slot);
        if (CanvasPanelSlot)
        {
            CanvasPanelSlot->SetSize(FVector2D(Length,15));
        }
    }
}

void UARPGProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (DummyValue != TrueValue)
    {
        DummyValue += (TrueValue - DummyValue) * InDeltaTime * 3;
        if (ProgressBar)
        {
            ProgressBar->SetPercent(DummyValue / TotalValue);
        }
    }
}