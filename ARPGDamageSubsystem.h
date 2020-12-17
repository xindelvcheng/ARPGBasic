// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGDamageSubsystem.generated.h"

class UDamageDetectRecord;

class AARPGCharacter;
class UCharacterStatusComponent;

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
	TWeakObjectPtr<USceneComponent> DamageCenterComponent;
	TWeakObjectPtr<UCharacterStatusComponent> InstigatorStatusComponent;
	TWeakObjectPtr<AController> InstigatorPlayerController;
	TArray<AActor*> HitActors;
	FTransform LastFrameDamageCenterTransform;
	FTransform CurrentFrameDamageCenterTransform;
	TArray<AActor*> ActorsToIgnore;

	TArray<TEnumAsByte<EObjectTypeQuery>> DetectObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};
	bool bDrawDebug = false;

	float BaseAttack;
	FDamageDetectDescriptionStruct DamageDetectDescriptionStruct;

	static UDamageDetectRecord* Create(USceneComponent* DamageCenterSceneComponent,
	                                   AARPGCharacter* InstigatorCharacter,
	                                   FDamageDetectedDelegate DamageDetectedDelegate,
	                                   FDamageDetectDescriptionStruct DamageDetectDescription = {});

	void Tick(float);

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

	UDamageDetectRecord* RegisterToDamageDetect(USceneComponent* DamageCenterSceneComponent,
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

UCLASS()
class UARPGDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	virtual EElementOverlapResultEnum ReinforceResult(EElementOverlapResultEnum ElementType){return EElementOverlapResultEnum::Irrelevant;}; 
};


UCLASS()
class UFireDamage : public UARPGDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UStoneDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UMetalDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UWaterDamage : public UDamageType
{
	GENERATED_BODY()
};

UCLASS()
class UElectricityDamage : public UDamageType
{
	GENERATED_BODY()
};