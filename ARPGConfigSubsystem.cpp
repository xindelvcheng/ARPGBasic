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
		const bool bNotConfig = BasicSettings->StatusWidgetClass.ToString().IsEmpty()
			|| BasicSettings->NotifyWidgetClass.ToString().IsEmpty()
			|| BasicSettings->GameItemWidgetClass.ToString().IsEmpty()
			|| BasicSettings->PromptWidgetClass.ToString().IsEmpty()
			|| BasicSettings->LockTargetWidgetClass.ToString().IsEmpty()
			|| BasicSettings->AbilityConfigDataTable.IsValid();
		if (bNotConfig)
		{
			UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，ARPG未完成基本项目设置"), 15, FColor::Red);
		}

		AbilityConfigDataTable = BasicSettings->AbilityConfigDataTable.LoadSynchronous();
	}
}

UARPGConfigSubsystem* UARPGConfigSubsystem::Get(UWorld* World)
{
	if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGGameInstanceSubsystem>())
	{
		return World->GetGameInstance()->GetSubsystem<UARPGConfigSubsystem>();
	}

	return nullptr;
}
