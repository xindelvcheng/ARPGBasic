// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGDamageSubsystem.generated.h"

class AARPGCharacter;
class UCharacterStatusComponent;

USTRUCT()
struct FDamageDetectDescriptionStruct
{
	GENERATED_BODY()

	FVector DamageBoxHalfSizeInTrace = FVector(100, 100, 100);
	float DamageWeight = 1;
	float DamageBias = 0;
	float VelocityDamageBonusWeight = 0.01;
	TSubclassOf<UDamageType> DamageTypeClass;
	TArray<AActor*> ActorsToIgnore;
};

UCLASS()
class UDamageDetectRecord : public UObject
{
	GENERATED_BODY()

public:
	
	TArray<FHitResult> HitResults;
	TWeakObjectPtr<AARPGCharacter> Instigator;
	TWeakObjectPtr<USceneComponent> DamageCenter;
	TWeakObjectPtr<UCharacterStatusComponent> InstigatorStatusComponent;
	TWeakObjectPtr<AController> InstigatorPlayerController;
	TArray<AActor*> HitActors;
	FTransform LastFrameDamageCenterTransform;
	FTransform CurrentFrameDamageCenterTransform;

	TArray<TEnumAsByte<EObjectTypeQuery>> DetectObjectTypes{ObjectTypeQuery1, ObjectTypeQuery2, ObjectTypeQuery3};
	EDrawDebugTrace::Type DrawDebugType{EDrawDebugTrace::None};
#if WITH_EDITOR	
	bool bDrawDebug = false;
#endif
	bool CauseDamage = true;
	float BaseAttack;
	FDamageDetectDescriptionStruct 	DamageDetectDescriptionStruct;

	static UDamageDetectRecord* Create(TWeakObjectPtr<USceneComponent> DamageCenterSceneComponent,AARPGCharacter* InstigatorCharacter,FDamageDetectDescriptionStruct DamageDetectDescription = {});

	void Tick(float);

	DECLARE_DELEGATE_TwoParams(FDamageDetectedDelegate,UDamageDetectRecord*,FHitResult);
	FDamageDetectedDelegate DamageDetectedDelegate;

	virtual void OnDamageDetected(FHitResult HitResult);
};



/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGDamageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	TArray<UDamageDetectRecord*> Records;
protected:
	UFUNCTION()
    bool Tick(float DeltaSeconds);

public:
	static UARPGDamageSubsystem* Get(UWorld* World);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UDamageDetectRecord* RegisterToDamageDetect(TWeakObjectPtr<USceneComponent> DamageCenterSceneComponent, AARPGCharacter* InstigatorCharacter,FDamageDetectDescriptionStruct DamageDetectDescription = {});
	UDamageDetectRecord* RegisterToDamageDetect(TWeakObjectPtr<AActor> DamageCenter, AARPGCharacter* InstigatorCharacter,FDamageDetectDescriptionStruct DamageDetectDescription = {});

	void UnRegisterToDamageDetect(UDamageDetectRecord* Record);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamageDetectedEvent,UDamageDetectRecord*,Record,FHitResult,HitResult);

	UPROPERTY(BlueprintAssignable,Category="ARPGDamageSubsystem",DisplayName="OnDamageDetected")
	FDamageDetectedEvent DamageDetectedEvent;
};
