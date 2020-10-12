// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGWidgetsLab.h"



#include "ARPGBasicSettings.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"


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

bool UARPGPromptWidget::Initialize()
{
    Super::Initialize();

    if (!Button_Yes || !Button_No || !TextBlock_PromptText)
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("ARPGPromptWidget未合理设置"));
        return false;
    }

    Button_Yes->OnClicked.AddDynamic(this,&UARPGPromptWidget::OnClickButton_Yes);
    Button_No->OnClicked.AddDynamic(this,&UARPGPromptWidget::OnClickButton_No);
    return true;
}

void UARPGPromptWidget::OnClickButton_Yes()
{
    OnChooseYes.ExecuteIfBound();
    RemoveFromParent();
}

void UARPGPromptWidget::OnClickButton_No()
{
    OnChooseNo.ExecuteIfBound();
    RemoveFromParent();
}

void UARPGPromptWidget::SetPromptText(FText PromptText)
{
    TextBlock_PromptText->SetText(PromptText);
}

UShowPromptWidgetBlueprintNode* UShowPromptWidgetBlueprintNode::ShowPromptWidget(const UObject* WorldContextObject,FText PromptText)
{
    UShowPromptWidgetBlueprintNode* Node = NewObject<UShowPromptWidgetBlueprintNode>();
    Node->CreatePromptWidget(WorldContextObject->GetWorld(), PromptText);
    return Node;
}

void UShowPromptWidgetBlueprintNode::CreatePromptWidget(const UWorld* World, FText PromptText)
{
    if (UARPGBasicSettings::Get())
    {
        PromptWidgetClass = LoadClass<UARPGPromptWidget>(
            nullptr, *UARPGBasicSettings::Get()->PromptWidgetClass.ToSoftObjectPath().ToString());

        verifyf(PromptWidgetClass, TEXT("没有设定PromptWidgetClass"))
    }

    UARPGPromptWidget* PromptWidget = Cast<UARPGPromptWidget>(CreateWidget(UGameplayStatics::GetPlayerController(World,0),PromptWidgetClass));
    PromptWidget->SetPromptText(PromptText);
    PromptWidget->OnChooseYes.BindLambda([&]()
    {
        Yes.Broadcast();
    });
    PromptWidget->OnChooseNo.BindLambda([&]()
    {
        No.Broadcast();
        
    });
    PromptWidget->AddToViewport();
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

bool UARPGLockTargetWidget::Initialize()
{
    Super::Initialize();

    return true;
}

void UARPGLockTargetWidget::SetLockIconScreenPosition(FVector2D ScreenPosition)
{
    if (Image_LockIcon)
    {
        UCanvasPanelSlot* CanvasPanel = Cast<UCanvasPanelSlot>(Image_LockIcon->Slot);
        if (CanvasPanel)
        {
            CanvasPanel->SetPosition(ScreenPosition);
        }
    }
}
