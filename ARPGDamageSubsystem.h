// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGDamageSubsystem.generated.h"

class UARPGDamageSubsystem;
class UDamageDetectRecord;

class AARPGCharacter;
class UCharacterStatusComponent;
class UARPGDamageBoxComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FDamageDetectedEvent, UDamageDetectRecord*, FHitResult);
DECLARE_DELEGATE_TwoParams(FDamageDetectedDelegate, UDamageDetectRecord*, FHitResult);

USTRUCT()
struct FDamageDetectDescriptionStruct
{
	GENERATED_BODY()

	bool bUseDamageCenterComponentCurrentBoundsAsDetectBound = true;
	/*DamageBoxHalfSizeInTrace在bUseDamageCenterComponentCurrentBoundsAsDetectBound = true（默认）时无效*/
	FVector DamageBoxHalfSizeInTrace = FVector(100, 100, 100);
	bool CauseDamage = true;
	float DamageWeight = 1;
	float DamageBias = 0;
	float VelocityDamageBonusWeight = 0.01;
	TSubclassOf<UDamageType> DamageTypeClass;
};

UCLASS()
class UDamageDetectRecord : public UObject
{
	GENERATED_BODY()

public:

	TArray<FHitResult> HitResults;
	TWeakObjectPtr<AARPGCharacter> Instigator;
	TWeakObjectPtr<UARPGDamageBoxComponent> DamageCenterComponent;
	TWeakObjectPtr<UCharacterStatusComponent> InstigatorStatusComponent;
	TWeakObjectPtr<AController> InstigatorPlayerController;
	TArray<AActor*> HitActors;
	FTransform LastFrameDamageCenterTransform;
	FTransform CurrentFrameDamageCenterTransform;
	TArray<AActor*> ActorsToIgnore;
	TWeakObjectPtr<UARPGDamageSubsystem> ARPGDamageSubsystem;

	TArray<TEnumAsByte<EObjectTypeQuery>> DetectObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};
	bool bDrawDebug = false;

	float BaseAttack;
	FDamageDetectDescriptionStruct DamageDetectDescriptionStruct;

	float TimePassed;

	static UDamageDetectRecord* Create(UARPGDamageBoxComponent* DamageCenterSceneComponent,
	                                   AARPGCharacter* InstigatorCharacter,
	                                   FDamageDetectedDelegate DamageDetectedDelegate,
	                                   FDamageDetectDescriptionStruct DamageDetectDescription = {});

	void Tick(float DeltaTime);

	FDamageDetectedEvent DamageDetectedEvent;

	virtual void OnDamageDetected(FHitResult HitResult);
};


/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGDamageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UDamageDetectRecord*> Records;
protected:
	UFUNCTION()
	bool Tick(float DeltaSeconds);

public:
	static UARPGDamageSubsystem* Get(UWorld* World);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UDamageDetectRecord* RegisterToDamageDetect(UARPGDamageBoxComponent* DamageCenterSceneComponent,
	                                            AARPGCharacter* InstigatorCharacter,
	                                            FDamageDetectedDelegate DelegateOnDamageDetected,
	                                            FDamageDetectDescriptionStruct DamageDetectDescription = {});
	UDamageDetectRecord* RegisterToDamageDetect(AActor* DamageCenter,
	                                            AARPGCharacter* InstigatorCharacter,
	                                            FDamageDetectedDelegate DelegateOnDamageDetected,
	                                            FDamageDetectDescriptionStruct DamageDetectDescription = {});

	void UnRegisterToDamageDetect(UDamageDetectRecord* Record);
};

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