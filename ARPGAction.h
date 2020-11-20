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


public:

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter);

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    virtual void ActivateAction(AARPGCharacter* Target = nullptr);

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    virtual void Interrupt(AARPGCharacter* Causer);

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    void FinishAction();

    UFUNCTION(BlueprintNativeEvent)
    bool CheckConditionAndPayCost();

    DECLARE_DELEGATE_OneParam(FActionFinishDelegate,AARPGAction*);
    FActionFinishDelegate OnActionFinished;
};

UCLASS(Blueprintable)
class AARPGMontageAction : public AARPGAction
{
    GENERATED_BODY()

    UPROPERTY()
    UAnimInstance* AttachedCharacterAnimInstance;
    


protected:

    virtual void ActivateAction(AARPGCharacter* Target) override;

    UFUNCTION()
    void BindToMontageBegin(UAnimMontage* Montage);
    UFUNCTION()
    void BindToMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
    UFUNCTION()
    void BindToMontageStop(UAnimMontage* Montage, bool bInterrupted);

    virtual void OnMontageBegin(UAnimMontage* Montage){};
    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload){};
    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted){};
    
    virtual void Interrupt(AARPGCharacter* Causer) override;

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter) override;

public:
    UPROPERTY(BlueprintReadWrite,Category="ARPGMontageAction")
    UAnimMontage* ActionMontage;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGMontageAction")
    FName StartSectionName = NAME_None;

    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGMontageAction")
    float PlayRate = 1;

    int MontageInstanceID;

    UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMontageBegin")
    void BPFunc_OnMontageBegin(AARPGCharacter* Character);
    UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMontageNotify")
    void BPFunc_OnMontageNotify(FName NotifyName, AARPGCharacter* Character);
    UFUNCTION(BlueprintImplementableEvent,DisplayName="OnMontageStop")
    void BPFunc_OnMontageStop(AARPGCharacter* Character);
    UFUNCTION(BlueprintImplementableEvent,DisplayName="OnInterrupt")
    void BPFunc_Interrupt(AARPGCharacter* Character);
};

UCLASS(Blueprintable)
class AARPGSingleMontageAction : public AARPGMontageAction
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSingleMontageAction")
    UAnimMontage* ActionMontageAsset;

    virtual void ActivateAction(AARPGCharacter* Target) override;
};

UCLASS(Blueprintable)
class AARPGMeleeAttackAction : public AARPGMontageAction
{
    GENERATED_BODY()

    int MeleeAttackIndex;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGMeleeAttackCollection",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> MeleeAttackMontages;

    virtual void ActivateAction(AARPGCharacter* Target) override;

    virtual void OnMontageBegin(UAnimMontage* Montage) override;

    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;

    virtual void Interrupt(AARPGCharacter* Causer) override;
};

UCLASS(Blueprintable)
class AARPGMultiMontageAction : public AARPGMontageAction
{
    GENERATED_BODY()

    int ActionIndex;
    FTimerHandle ResetTimerHandle;
    float ResetDelay = 1;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGMultiMontageAction",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> ActionMontages;

    virtual void ActivateAction(AARPGCharacter* Target) override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;
};
