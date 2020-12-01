// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGWidgetsLab.h"


#include "ARPGCharacter.h"
#include "ARPGBasicSettings.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "CharacterStatusComponent.h"
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
	if (ProgressBar)
	{
		if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ProgressBar->Slot))
		{
			OriginalPixelSize = UARPGGameInstanceSubsystem::GetRelativeSize(OriginalRelativeSize);
			CanvasPanelSlot->SetSize(OriginalPixelSize);
		}
		else
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("UARPGProgressBar只能放置于Canvas上，否则无法自动调整大小"));
		}
	}

	return true;
}

void UARPGProgressBar::SetPercent(float Current, float Total)
{
	TrueValue = Current;
	TotalValue = Total;

	const float LengthFactor = log(1 + TotalValue);
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ProgressBar->Slot))
	{
		CanvasPanelSlot->SetSize(OriginalPixelSize * FVector2D(LengthFactor, 1));
	}
}


void UARPGProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (DummyValue != TrueValue)
	{
		DummyValue += (TrueValue - DummyValue) * InDeltaTime * 3;
		if (ProgressBar)
		{
			ProgressBar->SetPercent(DummyValue / TotalValue);
		}
	}
}

#if WITH_EDITOR
void UARPGProgressBar::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UARPGProgressBar, OriginalPixelSize))
	{
		OriginalRelativeSize = OriginalPixelSize / UARPGGameInstanceSubsystem::GetScreenSize();
	}
	else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(
		UARPGProgressBar, OriginalRelativeSize))
	{
		OriginalPixelSize = OriginalRelativeSize * UARPGGameInstanceSubsystem::GetScreenSize();
	}
	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(ProgressBar->Slot))
	{
		CanvasPanelSlot->SetSize(OriginalPixelSize);
	}
}
#endif

bool UARPGPromptWidget::Initialize()
{
	Super::Initialize();

	if (!Button_Yes || !Button_No || !TextBlock_PromptText)
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("ARPGPromptWidget未合理设置"));
		return false;
	}

	Button_Yes->OnClicked.AddDynamic(this, &UARPGPromptWidget::OnClickButton_Yes);
	Button_No->OnClicked.AddDynamic(this, &UARPGPromptWidget::OnClickButton_No);
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

UShowPromptWidgetBlueprintNode* UShowPromptWidgetBlueprintNode::ShowPromptWidget(
	const UObject* WorldContextObject, FText PromptText)
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

	UARPGPromptWidget* PromptWidget = Cast<UARPGPromptWidget>(
		CreateWidget(UGameplayStatics::GetPlayerController(World, 0), PromptWidgetClass));
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


bool UARPGLockTargetWidget::Initialize()
{
	Super::Initialize();

	return true;
}

void UARPGLockTargetWidget::SetLockIconScreenPosition(FVector2D ScreenPosition)
{
	if (UCanvasPanelSlot* CanvasPanel = Cast<UCanvasPanelSlot>(Image_LockIcon->Slot))
	{
		CanvasPanel->SetPosition(ScreenPosition);
	}
}

bool UARPGEnemyHPBarWidget::Initialize()
{
	Super::Initialize();
	if (!ProgressBar_HP)
	{
		return false;
	}

	return true;
}

void UARPGEnemyHPBarWidget::BindToCharacter(AARPGCharacter* Character)
{
	if (Character && Character->GetCharacterStatusComponent())
	{
		if (ProgressBar_HP)
		{
			ProgressBar_HP->SetPercent(
				static_cast<float>(Character->GetCurrentHP()) / static_cast<float>(Character->GetMaxHP()));
		}
		Character->GetCharacterStatusComponent()->OnCharacterPropertyChanged.AddDynamic(
			this, &UARPGEnemyHPBarWidget::OnCharacterStatusChanged);
	}
}

void UARPGEnemyHPBarWidget::OnCharacterStatusChanged(ECharacterProperty ChangedProperty, int CurrentValue,
                                                     int TotalValue, int DeltaValue)
{
	if (ChangedProperty == ECharacterProperty::CurrentHP && ProgressBar_HP)
	{
		ProgressBar_HP->SetPercent(static_cast<float>(CurrentValue) / static_cast<float>(TotalValue));
	}
}
