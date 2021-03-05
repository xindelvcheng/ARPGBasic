// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActorMovement.h"
#include "GameFramework/Character.h"
#include "ARPGSword.generated.h"

class AARPGCharacter;
class UARPGDamageBoxComponent;

UENUM(BlueprintType)
enum class ESwordStateEnum:uint8
{
	Idle,
	Combat
};

UENUM(BlueprintType)
enum class ECircleDirectionEnum:uint8
{
	Clockwise,
	Anticlockwise
};

/**
 *剑灵，本身是武器也是角色
 */
UCLASS()
class AARPGSword : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AARPGSword",meta=(AllowPrivateAccess))
	UARPGDamageBoxComponent* DamageBox;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AARPGSword",meta=(AllowPrivateAccess))
	UARPGActorMovementComponent* MovementComponent;

	ECircleDirectionEnum CircleDirection = ECircleDirectionEnum::Clockwise;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AARPGSword",meta=(AllowPrivateAccess))
	UParticleSystem* TrailFXResource;

	UPROPERTY()
	UParticleSystemComponent* TrailFXParticleSystemComponent;

	ESwordStateEnum SwordState = ESwordStateEnum::Idle;

	/*TODO:跟随攻击*/
	void Attack();
	void Defend();

	float Timer = 0;
public:
	// Sets default values for this character's properties,this is before any other initialize method so don't try to access property of other object
	AARPGSword();

	FORCEINLINE AARPGCharacter* GetOwnerCharacter() const { return GetOwner<AARPGCharacter>(); }

	USkeletalMeshComponent* GetOwnerCharacterSkeletalMesh() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
