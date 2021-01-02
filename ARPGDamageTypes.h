// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGDamageTypes.generated.h"



/**
 * 
 */

UENUM()
enum class EElementOverlapResultEnum:uint8
{
	Irrelevant,Enhance,Weaken
};

UCLASS(BlueprintType)
class UMetalDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UWoodDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UWaterDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UIceDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UFireDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UStoneDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class UElectricityDamage : public UDamageType
{
	GENERATED_BODY()
};