// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGamingMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "ARPGBasicSettings.h"
#include "ARPGMainCharacter.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGPlayerController.h"
#include "ARPGStaticFunctions.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"
#include "Blueprint/WidgetTree.h"

UARPGGameMenuWidget::UARPGGameMenuWidget()
{
}

bool UARPGGameMenuWidget::Initialize()
{
	return Super::Initialize();
}

void UARPGGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TArray<UARPGPageContentItemWidget*> UARPGSpellManagerPageWidget::SetupContent()
{
	TArray<UARPGPageContentItemWidget*> Content;
	if (!GetOwnerCharacter())
	{
		UARPGStaticFunctions::PrintLogToScreen(TEXT("编辑器状态下无法查看角色信息"));
		return TArray<UARPGPageContentItemWidget*>{};	
	}

	if (UARPGCharacterCombatComponent* CharacterCombatComponent = GetOwnerCharacter()->GetCharacterCombatComponent())
	{
		for (AARPGCastAction* Action : CharacterCombatComponent->GetAbilityActions())
		{
			UARPGSingleButtonItemWidget* ItemWidget = CreateWidget<UARPGSingleButtonItemWidget>(
				this, GetItemWidgetClass());
			Content.Add(ItemWidget);

			ItemWidget->SetItemName(Action->GetGameItemDisplayName());

			if (!Action->GetIsInBag())
			{
				ItemWidget->SetItemOperationText(FText::FromString(TEXT("安装")));
			}
			else
			{
				ItemWidget->SetItemOperationText(FText::FromString(TEXT("卸载")));
			}

			ButtonSpellMap.Add(ItemWidget->GetOperationButton(), Action);
			ItemWidget->GetOperationButton()->OnButtonClick().AddLambda([this,ItemWidget](UARPGButton* Button)
			{
				if (UARPGSpellsManagerComponent* SpellsManagerComponent = GetOwnerCharacter()->
					GetCharacterSpellsManagerComponent())
				{
					if (AARPGCastAction* CastAction = ButtonSpellMap.FindRef(Button))
					{
						if (!CastAction->GetIsInBag())
						{
							SpellsManagerComponent->AddItem(CastAction);
							CastAction->SetIsInBag(true);
							ItemWidget->SetItemOperationText(FText::FromString(TEXT("卸载")));
						}
						else
						{
							SpellsManagerComponent->RemoveItem(CastAction);
							CastAction->SetIsInBag(false);
							ItemWidget->SetItemOperationText(FText::FromString(TEXT("安装")));
						}
					}
				}
			});
		}
	}
	return Content;
}

TArray<UARPGPageContentItemWidget*> UARPGSystemWidget::SetupContent()
{
	TArray<UARPGPageContentItemWidget*> Content;

	if (UARPGSingleButtonItemWidget* ItemWidget = CreateWidget<UARPGSingleButtonItemWidget>(
		this, GetItemWidgetClass())
	)
	{
		ItemWidget->SetItemName(FText::FromString(TEXT("退出游戏")));
		ItemWidget->GetOperationButton()->OnButtonClick().AddLambda([this](UButton*)
		{
			UKismetSystemLibrary::QuitGame(GetOwnerPlayerController(), GetOwnerPlayerController(),
			                               EQuitPreference::Quit, true);
		});
		Content.Add(ItemWidget);
	}
	return Content;
}
