// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGUISubsystem.h"

#include "ARPGCoreSubsystem.h"

void UARPGUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

template<typename T>
T* UARPGButton::GetParentWidget()
{
	UObject* Parent = GetOuter();
	T* Result = Cast<T>(Parent);
	while (Parent && !Result)
	{
		Parent = Parent->GetOuter();
		Result = Cast<T>(Parent);
	}
	return Result;
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
	WidgetSwitcher_Content->ClearChildren();
	
	for (FTabWidgetPageConfig TabWidgetPageConfig : TabWidgetPageConfigs)
	{
		UARPGTitleWidget* TitleWidget = CreateWidget<UARPGTitleWidget>(this, TitleClass);
		if (!TitleWidget)
		{
			continue;
		}
		TitleWidget->SetTitleName(TabWidgetPageConfig.TitleName);
		TitleWidget->SetTitleIcon(TabWidgetPageConfig.TitleIcon);
		TitleWidget->SetOwnerCharacter(GetOwnerCharacter());
		TitleWidget->SetOwnerPlayerController(GetOwnerPlayerController());
		HorizontalBox_TitleBar->AddChildToHorizontalBox(TitleWidget);

		UARPGPageWidget* PageWidget = CreateWidget<UARPGPageWidget>(this, TabWidgetPageConfig.PageClass);
		
		if (!PageWidget)
		{
			continue;
		}
		PageWidget->SetOwnerCharacter(GetOwnerCharacter());
		PageWidget->SetOwnerPlayerController(GetOwnerPlayerController());
		PageWidget->Show();
		
		AddPage(TitleWidget->GetTriggerPageSwitchButton(), PageWidget);
	}
}

void UARPGTabWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UARPGTitleWidget* TitleWidget = Cast<UARPGTitleWidget>(HorizontalBox_TitleBar->GetChildAt(0));
	TitleWidget->GetTriggerPageSwitchButton()->OnButtonClick().Broadcast(TitleWidget->GetTriggerPageSwitchButton());
}

void UARPGTabWidget::AddPage(UARPGButton* TriggerButton, UARPGPageWidget* PageWidget)
{
	WidgetSwitcher_Content->AddChild(PageWidget);
	TriggerButton->OnButtonClick().AddLambda([&](UARPGButton* Button)
	{
		WidgetSwitcher_Content->SetActiveWidgetIndex(HorizontalBox_TitleBar->GetChildIndex(Button->GetParentWidget<UARPGTitleWidget>()));
		Button->GetParentWidget<UARPGTitleWidget>()->Show();
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
