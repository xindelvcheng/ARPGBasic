// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "GameFramework/Character.h"
#include "ARPGNonPlayerCharacter.generated.h"

UCLASS()
class  AARPGNonPlayerCharacter : public AARPGCharacter
{
	GENERATED_BODY()

	
	bool IsRigid;
	
	
	int NormalAttackIndex = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> NormalAttackMontages;

	int RemoteAttackIndex = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> RemoteAttackMontages;

	int AbilityIndex = 0;
		
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> AbilityMontages;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNonPlayerCharacter",meta=(AllowPrivateAccess=true))
	UAnimMontage* AnimMontage_Rigid;
	
	UPROPERTY()
	UAnimInstance* AnimInstance;

    UFUNCTION()
    void OnMontageBegin(UAnimMontage* Montage);

	UFUNCTION()
	void OnMontageNotify(FName  NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageStop(UAnimMontage* Montage, bool bInterrupted);
public:
	// Sets default values for this character's properties
	AARPGNonPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
	virtual void TryToNormalAttack();

	UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    virtual void TryToRangeAttack();

	UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
    virtual void TryToUseAbility();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackEvent);
	
	UPROPERTY(BlueprintCallable,BlueprintAssignable,Category="ARPGNonPlayerCharacter")
	FAttackEvent OnAttackOrAbilityComplete;

	
	UFUNCTION(BlueprintCallable,Category="ARPGNonPlayerCharacter")
	void CauseRigid(float Duration);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRigidEvent,float,Duration);
	UPROPERTY(BlueprintAssignable,Category="ARPGNonPlayerCharacter")
	FRigidEvent OnRigid;
	FRigidEvent OnResumeFromRigid;

	

	FTimerHandle RigidTimerHandle;
	FTimerDelegate RigidTimerDelegate;
};




UCLASS()
class  AARPGBossAI : public AARPGNonPlayerCharacter
{
	GENERATED_BODY()
};
