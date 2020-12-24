// Fill out your copyright notice in the Description page of Project Settings.



#include "ARPGConfigSubsystem.h"


#include "ARPGBasicSettings.h"
#include "ARPGGameInstanceSubsystem.h"
#include "AssetRegistryModule.h"
#include "Engine/DataTable.h"

void UARPGConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 检查是否完成基本项目设置


	if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
	{
		StatusWidgetClass = BasicSettings->StatusWidgetClass.LoadSynchronous();
		NotifyWidgetClass = BasicSettings->NotifyWidgetClass.LoadSynchronous();
		GameItemWidgetClass = BasicSettings->GameItemWidgetClass.LoadSynchronous();
		PromptWidgetClass = BasicSettings->PromptWidgetClass.LoadSynchronous();
		LockTargetWidgetClass = BasicSettings->LockTargetWidgetClass.LoadSynchronous();
		AbilityConfigDataTable = BasicSettings->AbilityConfigDataTable.LoadSynchronous();

		DefaultGameplayIcon = BasicSettings->DefaultGameplayIcon.LoadSynchronous();
		DefaultGameItemIcon = BasicSettings->DefaultGameItemIcon.LoadSynchronous();
		DefaultAbilityIcon = BasicSettings->DefaultAbilityIcon.LoadSynchronous();
		
		const bool bBasicConfigComplete = StatusWidgetClass
			&& NotifyWidgetClass
			&& GameItemWidgetClass
			&& PromptWidgetClass
			&& LockTargetWidgetClass
			&& AbilityConfigDataTable
			&& DefaultGameplayIcon
			&& DefaultGameItemIcon
			&& DefaultAbilityIcon;
#if WITH_EDITOR
		if (!bBasicConfigComplete)
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，ARPG未完成基本项目设置"), 15, FColor::Red);
		}
#endif
	}
}
UARPGConfigSubsystem* UARPGConfigSubsystem::Get(UWorld* World)
{
	if (World && World->GetGameInstance())
	{
		return World->GetGameInstance()->GetSubsystem<UARPGConfigSubsystem>();
	}

	return nullptr;
}
