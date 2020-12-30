// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGUISubsystem.h"

void UARPGUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UARPGWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	OwnerCharacter = Cast<AARPGCharacter>(GetOwningPlayerPawn());
	OwnerPlayerController = Cast<AARPGPlayerController>(GetOwningPlayer());
}

void UARPGWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UARPGWidget::SetOwnerCharacter(AARPGCharacter* NewOwnerCharacter)
{
	this->OwnerCharacter = NewOwnerCharacter;
}

void UARPGWidget::SetOwnerPlayerController(AARPGPlayerController* NewOwnerPlayerController)
{
	this->OwnerPlayerController = NewOwnerPlayerController;
}

void UARPGTabWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	/*NativePreConstruct似乎修改一次属性就调用一次*/
	HorizontalBox_TitleBar->ClearChildren();
	TabButtonsMap.Empty();
	
	for (FTabWidgetPageConfig TabWidgetPageConfig : TabWidgetPageConfigs)
	{
		UARPGTitleWidget* TitleWidget = CreateWidget<UARPGTitleWidget>(this, TitleClass);
		TitleWidget->SetTitleName(TabWidgetPageConfig.TitleName);
		TitleWidget->SetTitleIcon(TabWidgetPageConfig.TitleIcon);
		TitleWidget->SetOwnerCharacter(GetOwnerCharacter());
		TitleWidget->SetOwnerPlayerController(GetOwnerPlayerController());
		HorizontalBox_TitleBar->AddChildToHorizontalBox(TitleWidget);
		
		UARPGPageWidget* PageWidget = CreateWidget<UARPGPageWidget>(this, TabWidgetPageConfig.PageClass);
		PageWidget->SetOwnerCharacter(GetOwnerCharacter());
		PageWidget->SetOwnerPlayerController(GetOwnerPlayerController());
		PageWidget->AddToViewport();
		AddPage(TitleWidget->GetTriggerPageSwitchButton(), PageWidget);
	}
}

void UARPGTabWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//创建该窗体时自动激活第一页
	if (!CurrentActiveWidget)
	{
		if (UARPGTitleWidget* TitleWidget = Cast<UARPGTitleWidget>(HorizontalBox_TitleBar->GetChildAt(0)))
		{
			if (UARPGButton* SwitchButton = TitleWidget->GetTriggerPageSwitchButton())
			{
				SwitchButton->OnButtonClick().Broadcast(TitleWidget->GetTriggerPageSwitchButton());
			}
		}
	}
}

void UARPGTabWidget::AddPage(UARPGButton* TriggerButton, UARPGPageWidget* PageWidget)
{
	TabButtonsMap.Add(TriggerButton, PageWidget);
	TriggerButton->OnButtonClick().AddLambda([&](UARPGButton* Button)
	{
		if (UARPGPageWidget* AssociatedWidget = TabButtonsMap.FindRef(Button))
		{
			if (AssociatedWidget != CurrentActiveWidget)
			{
				if (CurrentActiveWidget)
				{
					CurrentActiveWidget->Hide();
				}
				AssociatedWidget->Show();
			}
		}
	});
}

void UARPGPageWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UARPGPageWidget::Show()
{
	Super::Show();
	
	ScrollBox_Content->ClearChildren();
	for (auto ItemWidget : SetupContent())
	{
		ScrollBox_Content->AddChild(ItemWidget);
	}
}

void UARPGPageWidget::Hide()
{
	Super::Hide();
}

bool UARPGTitleWidget::Initialize()
{
	Super::Initialize();

	if (!TextBlock_TitleName || !Button_TriggerPageSwitch)
	{
		return false;
	}
	return true;
}

void UARPGTitleWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!Image_TitleIcon->Brush.GetResourceObject())
	{
		Image_TitleIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}
