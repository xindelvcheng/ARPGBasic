// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGConfigSubsystem.h"


#include "ARPGBasicSettings.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"
#include "AssetRegistryModule.h"
#include "Engine/DataTable.h"


void UARPGConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 检查是否完成基本项目设置
	CheckARPGBasicConfig();
}

void UARPGConfigSubsystem::CheckARPGBasicConfig()
{
	if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
	{
		AbilityConfigDataTable = BasicSettings->AbilityConfigDataTable.LoadSynchronous();

		StatusWidgetClass = BasicSettings->StatusWidgetClass.LoadSynchronous();
		NotifyWidgetClass = BasicSettings->NotifyWidgetClass.LoadSynchronous();
		GameItemWidgetClass = BasicSettings->GameItemWidgetClass.LoadSynchronous();
		PromptWidgetClass = BasicSettings->PromptWidgetClass.LoadSynchronous();
		LockTargetWidgetClass = BasicSettings->LockTargetWidgetClass.LoadSynchronous();
		AimPromptActorClass = BasicSettings->AimPromptActorClass.LoadSynchronous();

		DefaultGameplayIcon = BasicSettings->DefaultGameplayIcon.LoadSynchronous();
		DefaultGameItemIcon = BasicSettings->DefaultGameItemIcon.LoadSynchronous();
		DefaultAbilityIcon = BasicSettings->DefaultAbilityIcon.LoadSynchronous();

		

		const bool bBasicConfigComplete = StatusWidgetClass
            && NotifyWidgetClass
            && GameItemWidgetClass
            && PromptWidgetClass
            && LockTargetWidgetClass
            /*&& AbilityConfigDataTable*/
            && DefaultGameplayIcon
            && DefaultGameItemIcon
            && DefaultAbilityIcon
            && AimPromptActorClass;
#if WITH_EDITOR
		if (!bBasicConfigComplete)
		{
			UARPGStaticFunctions::PrintLogToScreen(TEXT("错误，ARPG未完成基本项目设置"), 15, FColor::Red);
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
