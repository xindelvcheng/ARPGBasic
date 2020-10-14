#pragma once

#include "CoreMinimal.h"

#include "ARPGAction.generated.h"

class AARPGCharacter;

UCLASS(Blueprintable)
class AARPGAction : public AActor
{
    GENERATED_BODY()


protected:
    
    UPROPERTY(BlueprintReadOnly,Category="ARPGAction")
    AARPGCharacter* OwningCharacter;

    UFUNCTION(BlueprintImplementableEvent,DisplayName="ActionActive")
    void BPFunc_Active(AARPGCharacter* Target = nullptr);
    UFUNCTION(BlueprintCallable,DisplayName="Action Finish")
    void FinishAction();

public:

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter);

    virtual void ActivateAction(AARPGCharacter* Target = nullptr);
    virtual void Interrupt(AARPGCharacter* Causer);

    DECLARE_DELEGATE(FActionFinishDelegate);
    FActionFinishDelegate OnActionFinished;
};

UCLASS(Blueprintable)
class AARPGSimpleMontageAction : public AARPGAction
{
    GENERATED_BODY()

    UPROPERTY()
    UAnimInstance* AttachedCharacterAnimInstance;

protected:

    virtual void ActivateAction(AARPGCharacter* Target) override;
    
    UFUNCTION()
    virtual void OnMontageBegin(UAnimMontage* Montage)
    {
        // checkf(false, TEXT("This method should be override in child class"))
    };
    UFUNCTION()
    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
    {
        // checkf(false, TEXT("This method should be override in child class"))
    };
    UFUNCTION()
    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
    {
        if (Montage == ActionMontage)
        {
            FinishAction();
        }
    };

    virtual void Interrupt(AARPGCharacter* Causer) override;

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter) override;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNormalAttacks",meta=(AllowPrivateAccess=true))
    UAnimMontage* ActionMontage;
};

UCLASS(Blueprintable)
class AARPGMeleeAttackAction : public AARPGSimpleMontageAction
{
    GENERATED_BODY()

    int MeleeAttackIndex;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGNormalAttacks",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> MeleeAttackMontages;

    virtual void ActivateAction(AARPGCharacter* Target) override;

    virtual void OnMontageBegin(UAnimMontage* Montage) override;

    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;

    virtual void Interrupt(AARPGCharacter* Causer) override;
};
