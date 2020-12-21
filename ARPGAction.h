#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"

#include "ARPGAction.generated.h"

class AARPGCharacter;
class AARPGAction;

UCLASS(Blueprintable)
class AARPGAction : public AARPGActor
{
    GENERATED_BODY()

    DECLARE_DELEGATE_OneParam(FActionFinishDelegate, AARPGAction*);

    DECLARE_MULTICAST_DELEGATE_OneParam(FActionFinishEvent, AARPGAction*);
    FActionFinishEvent ActionFinishedEvent;
protected:

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGAction")
    int ExclusiveGroupID = 0;

    UFUNCTION(BlueprintImplementableEvent,DisplayName="ActionActive")
    void BPFunc_Active();

    virtual void OnActionActivate();

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    void FinishAction();
    
    virtual void OnActionFinished(AARPGAction* Action){};

    bool CheckConditionAndPayCost();

    UFUNCTION(BlueprintNativeEvent,DisplayName="CheckConditionAndPayCost")
    bool BPFunc_CheckConditionAndPayCost();

public:
    
    int GetActionExclusiveGroupID()const{return ExclusiveGroupID;}

    virtual void InitWithOwningCharacter(AARPGCharacter* NewOwningCharacter);

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    bool TryToActivateAction(AARPGCharacter* User = nullptr, AARPGCharacter* Target = nullptr);

    UFUNCTION(BlueprintCallable,Category="ARPGAction")
    virtual void Interrupt(AARPGCharacter* Causer);

    template<typename  T>
    static T* CreateARPGAction(TSubclassOf<AARPGAction> ActionClass,
                                         AARPGCharacter* ActionOwnerCharacter,FActionFinishDelegate ActionFinishedDelegate, int ActionExclusiveGroupID=0);

    template<typename  T>
    static T* CreateARPGAction(TSubclassOf<AARPGAction> ActionClass,
                                         AARPGCharacter* ActionOwnerCharacter,FActionFinishDelegate ActionFinishedDelegate, FTransform Transform,int ActionExclusiveGroupID=0);

    virtual FActionFinishEvent& OnActionFinishedEvent()
    {
        return ActionFinishedEvent;
    }
};


UCLASS(Blueprintable)
class AARPGMontageAction : public AARPGAction
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    UAnimInstance* AttachedCharacterAnimInstance;

    virtual void OnActionActivate() override;

    UFUNCTION()
    void BindToMontageBegin(UAnimMontage* Montage);
    UFUNCTION()
    void BindToMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
    UFUNCTION()
    void BindToMontageStop(UAnimMontage* Montage, bool bInterrupted);

    virtual void OnMontageBegin(UAnimMontage* Montage)
    {
    };

    virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
    {
    };

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted)
    {
    };

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

    virtual void OnActionActivate() override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;
};

UCLASS(Blueprintable)
class AARPGMeleeAttackAction : public AARPGMontageAction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly,Category="ARPGMeleeAttackAction",meta=(AllowPrivateAccess))
    int MeleeAttackIndex;

public:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGMeleeAttackCollection",meta=(AllowPrivateAccess=true))
    TArray<UAnimMontage*> MeleeAttackMontages;

    virtual void OnActionActivate() override;

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

    virtual void OnActionActivate() override;

    virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;
};


UCLASS(Blueprintable)
class AARPGBuff : public AARPGAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPG Buff",meta=(AllowPrivateAccess))
    float BuffDuration = 30;
    FTimerHandle BuffTimerHandle;

    virtual void OnActionActivate() override;

public:
    void SetDuration(const float Duration){BuffDuration = Duration;}
};
