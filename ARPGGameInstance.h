// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ARPGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FPreLoadMapDelegate);
	DECLARE_MULTICAST_DELEGATE(FPostLoadMapDelegate);
	DECLARE_MULTICAST_DELEGATE(FSceneInitializeDelegate);
	DECLARE_MULTICAST_DELEGATE(FPreLoadArchiveDelegate);
	DECLARE_MULTICAST_DELEGATE(FPostLoadArchiveDelegate);
	

public:

	static FPreLoadMapDelegate& OnPreLoadMapDelegate()
	{
		static FPreLoadMapDelegate PreLoadMapDelegate;
		return PreLoadMapDelegate;
	}

	static FPostLoadMapDelegate& GetPostLoadMapDelegate()
	{
		static FPostLoadMapDelegate PostLoadMapDelegate;
		return PostLoadMapDelegate;
	}

	static FSceneInitializeDelegate& GetSceneInitializeDelegate()
	{
		static FSceneInitializeDelegate SceneInitializeDelegate;
		return SceneInitializeDelegate;
	}

	static FPreLoadArchiveDelegate& GetPreLoadArchiveDelegate()
	{
		static FPreLoadArchiveDelegate PreLoadArchiveDelegate;
		return PreLoadArchiveDelegate;
	}

	static FPostLoadArchiveDelegate& GetPostLoadArchiveDelegate()
	{
		static FPostLoadArchiveDelegate PostLoadArchiveDelegate;
		return PostLoadArchiveDelegate;
	}
};
