// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGArchiveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Engine/Classes/Engine/LevelStreaming.h"

#include "ARPGGameInstanceSubsystem.h"
#include "APRGGameSaver.h"
#include "ARPGBasicSettings.h"
#include "GameItemWidget.h"
#include "ARPGPlayerController.h"
#include "ARPGGameItemsManagerComponent.h"

#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"

TArray<FArchiveInfoStruct> UARPGArchiveSubsystem::GetArchiveInfos() const
{
	return ArchiveManager->ArchiveInfos;
}

void UARPGArchiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// 初始化存档列表
	ArchiveManager = Cast<UAPRGArchiveManager>(UGameplayStatics::LoadGameFromSlot(ArchiveManageSlot, 0));
	if (!ArchiveManager)
	{
		ArchiveManager = Cast<UAPRGArchiveManager>(
			UGameplayStatics::CreateSaveGameObject(UAPRGArchiveManager::StaticClass()));
		if (ArchiveManager)
		{
			for (int i = 0; i < 128; ++i)
			{
				FArchiveInfoStruct ArchiveInfoStruct;
				ArchiveInfoStruct.SlotName = FString(TEXT("Save")).Append(FString::FromInt(i));
				ArchiveInfoStruct.IsArchiveValid = false;
				ArchiveManager->ArchiveInfos.Emplace(std::move(ArchiveInfoStruct));
			}
			UGameplayStatics::SaveGameToSlot(ArchiveManager, ArchiveManageSlot, 0);
		}
	}
}

UARPGArchiveSubsystem* UARPGArchiveSubsystem::Get(UWorld* World)
{
	if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGArchiveSubsystem>())
	{
		return World->GetGameInstance()->GetSubsystem<UARPGArchiveSubsystem>();
	}

	return nullptr;
}

void UARPGArchiveSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UARPGArchiveSubsystem::SaveArchive(FString ArchiveName, FSaveGameDelegate CompleteDelegate)
{
	UARPGGameInstanceSubsystem::ShowNotify(GetWorld(),UARPGBasicSettings::Get()->Icons[0].LoadSynchronous(),FText::FromString(TEXT("正在保存游戏")),FText::FromString(TEXT("")));
	OnGameSaving.Broadcast();
	GameSaver = Cast<UAPRGGameSaver>(UGameplayStatics::CreateSaveGameObject(UAPRGGameSaver::StaticClass()));
	GameSaver->CurrentMap = CurrentStreamingLevelName;

	if (auto MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(this))
	{
		GameSaver->MainCharacterTransform = MainCharacter->GetTransform();
	}
	else
	{
		GameSaver->MainCharacterTransform = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetTransform();
	}

	GameSaver->Time = FDateTime::Now();
	
	FArchiveInfoStruct& ArchiveInfoStruct = *ArchiveManager->ArchiveInfos.FindByPredicate(
		[ArchiveName](FArchiveInfoStruct ArchiveInfoStruct)
		{
			return ArchiveInfoStruct.SlotName == ArchiveName;
		});

	//保存道具
	TArray<AActor*> GameItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameItem::StaticClass(), GameItems);
	for (AActor* Actor : GameItems)
	{
		AGameItem* GameItem = Cast<AGameItem>(Actor);
		if (GameItem)
		{
			FGameItemArchiveStruct GameItemArchiveStruct = {
				GameItem->GetClass(),
				GameItem->Number,
				GameItem->IsInBag,
				GameItem->GetActorTransform()
			};
			GameSaver->GameItems.Emplace(std::move(GameItemArchiveStruct));
		}
	}

	//保存角色
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPGCharacter::StaticClass(), OutActors);
	for (auto Actor : OutActors)
	{
		AARPGCharacter* Character = Cast<AARPGCharacter>(Actor);
		if (!Character || !Character->GetCharacterStatusComponent())
		{
			return false;
		}
		auto CharacterArchiveStruct = FCharacterArchiveStruct::MakeArchiveStruct(Character);
		GameSaver->Characters.Emplace(std::move(CharacterArchiveStruct));
	}
	FAsyncSaveGameToSlotDelegate AsyncSaveGameToSlotDelegate = FAsyncSaveGameToSlotDelegate::CreateLambda(
		[&](const FString&, const int32, bool)
		{
			ArchiveInfoStruct.IsArchiveValid = true;
			ArchiveInfoStruct.Timestamp = FDateTime::Now().ToString();

			UGameplayStatics::SaveGameToSlot(
				ArchiveManager, ArchiveManageSlot, 0);
			OnGameSaveSuccess.Broadcast();
			CompleteDelegate.ExecuteIfBound();
			UARPGGameInstanceSubsystem::ShowNotify(GetWorld(),UARPGBasicSettings::Get()->Icons[0].LoadSynchronous(),FText::FromString(TEXT("游戏已保存")),FText::FromString(TEXT("")));
		});
	UGameplayStatics::AsyncSaveGameToSlot(GameSaver, ArchiveName, 0, AsyncSaveGameToSlotDelegate);
	return true;
}

bool UARPGArchiveSubsystem::BPFunc_SaveArchive(UObject* WorldContextObject, int ArchiveIndex,
                                        FSaveGameDelegate CompleteDelegate)
{
	UARPGArchiveSubsystem* ArchiveSubsystem = Get(WorldContextObject->GetWorld());
	if (ArchiveSubsystem)
	{
		ArchiveIndex = FMath::Clamp(ArchiveIndex, 0, ArchiveSubsystem->ArchiveManager->ArchiveInfos.Num() - 1);
		if (ArchiveSubsystem->ArchiveManager->ArchiveInfos.IsValidIndex(ArchiveIndex))
		{
			ArchiveSubsystem->SaveArchive(ArchiveSubsystem->ArchiveManager->ArchiveInfos[ArchiveIndex].SlotName, CompleteDelegate);
			return true;
		}
	}
	return false;
}

void UARPGArchiveSubsystem::LoadArchive(FString ArchiveName, FSaveGameDelegate CompleteDelegate)
{
	OnGameLoading.Broadcast();

	const auto LoadGameFromSlotDelegate =
		FAsyncLoadGameFromSlotDelegate::CreateLambda([&](const FString& SlotName, const int32, USaveGame* SaveGame)
		{
			if (SaveGame)
			{
				GameSaver = Cast<UAPRGGameSaver>(SaveGame);

				if (GameSaver->CurrentMap != CurrentStreamingLevelName)
				{
					const FLatentActionInfo LatentInfo{0, -1,TEXT("OnLevelLoaded"), this};
					OnLevelLoading.Broadcast();
					UGameplayStatics::LoadStreamLevel(GetWorld(), GameSaver->CurrentMap, true, false, LatentInfo);

				}
				else
				{
					OnLevelLoaded();
				}
			}
		});
	UGameplayStatics::AsyncLoadGameFromSlot(ArchiveName, 0, LoadGameFromSlotDelegate);
}

void UARPGArchiveSubsystem::BPFunc_LoadArchive(UObject* WorldContextObject, int ArchiveIndex,
                                        FSaveGameDelegate CompleteDelegate)
{
	UARPGArchiveSubsystem* ArchiveSubsystem = Get(WorldContextObject->GetWorld());
	if (ArchiveSubsystem)
	{
		ArchiveIndex = FMath::Clamp(ArchiveIndex, 0, ArchiveSubsystem->ArchiveManager->ArchiveInfos.Num() - 1);
		if (ArchiveSubsystem->ArchiveManager->ArchiveInfos.IsValidIndex(ArchiveIndex))
		{
			ArchiveSubsystem->LoadArchive(ArchiveSubsystem->ArchiveManager->ArchiveInfos[ArchiveIndex].SlotName, CompleteDelegate);
		}
	}
}

void UARPGArchiveSubsystem::OnLevelLoaded()
{
	OnLevelLoadSuccess.Broadcast();
	if (auto MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld()))
	{
		MainCharacter->SetActorTransform(GameSaver->MainCharacterTransform);
	}


	//销毁所有角色
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPGCharacter::StaticClass(), OutActors);
	for (auto Actor : OutActors)
	{
		Actor->Destroy();
	}

	//销毁所有道具
	TArray<AActor*> GameItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameItem::StaticClass(), GameItems);
	for (AActor* Actor : GameItems)
	{
		Actor->Destroy();
	}

	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//从存档还原角色
	for (auto CharacterArchiveStruct : GameSaver->Characters)
	{
		auto CharacterClass = CharacterArchiveStruct.CharacterClassPath.LoadSynchronous();
		checkf(GetWorld(), TEXT("World is nullptr!"));
		auto Actor = GetWorld()->SpawnActor(CharacterClass, &CharacterArchiveStruct.CharacterTransform, ActorSpawnParameters);
		auto Character = Cast<AARPGCharacter>(Actor);
		if (Character->CharacterName == "MainCharacter")
		{
			if (auto MainCharacter = Cast<AARPGMainCharacter>(Character))
			{
				UARPGGameInstanceSubsystem::Get(GetWorld())->SetMainCharacter(MainCharacter);
				UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(Character);
			}
			else
			{
				UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，主角不是AARPGMainCharacter子类"));
			}
		}
		else
		{
			//对NPC生成AIController
			Character->SpawnDefaultController();
		}
		FCharacterArchiveStruct::LoadArchiveStruct(Character,CharacterArchiveStruct);

		UE_LOG(LogTemp, Warning, TEXT("%s角色已重新生成"), *Character->CharacterName.ToString());
	}


	//从存档还原道具
	TArray<AGameItem*> Bag;

	if (auto MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(this))
	{
		for (auto GameItemArchiveStruct : GameSaver->GameItems)
		{
			AGameItem* GameItem = Cast<AGameItem>(
				GetWorld()->SpawnActor(LoadClass<AGameItem>(nullptr, *GameItemArchiveStruct.GameItemClass.ToString()),
				                       &GameItemArchiveStruct.Transform, ActorSpawnParameters));
			if (GameItem)
			{
				if (GameItemArchiveStruct.IsInBag)
				{
					GameItem->Number = GameItemArchiveStruct.Number;
					Bag.Emplace(GameItem->BeTaken(MainCharacter));
				}
			}
		}
		MainCharacter->GetGameItemsManagerComponent()->SetBag(Bag);
		UE_LOG(LogTemp, Warning, TEXT("刷新物品和背包"));
	}
	UE_LOG(LogTemp, Warning, TEXT("存档加载完成"));
	OnGameLoadSuccess.Broadcast();
}
