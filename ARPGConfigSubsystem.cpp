// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGConfigSubsystem.h"


#include "ARPGBasicSettings.h"
#include "AssetRegistryModule.h"
#include "Engine/DataTable.h"

void UARPGConfigSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    
    TArray<FAssetData> AssetDatas;
    
    AssetRegistryModule.Get().GetAssetsByClass(FName(UDataTable::StaticClass()->GetName()),AssetDatas);
    UE_LOG(LogTemp,Warning,TEXT("加载配置文件"))
    
    for (FAssetData AssetData : AssetDatas)
    {
        if (AssetData.AssetName == "DT_CharactersConfig")
        {
            CharactersConfigDataTable = LoadObject<UDataTable>(this,*AssetData.ToSoftObjectPath().ToString());
        }
    }
}
