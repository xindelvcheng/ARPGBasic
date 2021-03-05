#pragma once

#include "CoreMinimal.h"

#include "ARPGActor.h"
#include "ARPGGameItem.h"
#include "ARPGCollectionComponent.h"


#include "ARPGAction.generated.h"

class AARPGCharacter;
class AARPGAction;
DECLARE_DELEGATE_OneParam(FActionFinishDelegate, AARPGAction*);

UENUM()
enum class EActionStateEnum:uint8 { Run, Stop };

UCLASS(Blueprintable, HideCategories=("GameItemFlag"))
class AARPGAction : public AARPGCollectableObject
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FActionFinishEvent, AARPGAction*);
	FActionFinishEvent ActionFinishedEvent;

	/*当前Action状态，Stop状态才能启动，Run状态才能停止*/
	EActionStateEnum ActionState;

	TWeakObjectPtr<AARPGCharacter> User;
	TWeakObjectPtr<AARPGCharacter> Target;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGAction")
	int ExclusiveGroupID = 0;

	virtual void OnActionActivate();

	virtual void OnActionFinished(AARPGAction* Action)
	{
	};

	virtual bool CheckActionActivateConditionAndPayCost();

	UFUNCTION(BlueprintNativeEvent, DisplayName="CheckConditionAndPayCost")
	bool BPFunc_CheckActionActivateConditionAndPayCost();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnActionActive")
	void BPFunc_OnActionActivate();

	virtual void BeginPlay() override;

public:
	AARPGAction();

	UFUNCTION(BlueprintCallable, Category="ARPGAction")
	bool TryToActivateAction(AARPGCharacter* NewUser = nullptr, AARPGCharacter* NewTarget = nullptr);

	/*结束Action，可能由Action自己调用（例如到时长了），也可以由外界调用打断Action*/
	UFUNCTION(BlueprintCallable, Category="ARPGAction")
	void FinishAction();

	int GetActionExclusiveGroupID() const { return ExclusiveGroupID; }


	template <typename T>
	static T* CreateARPGAction(TSubclassOf<AARPGAction> ActionClass,
	                           AARPGCharacter* ActionOwnerCharacter, FActionFinishDelegate ActionFinishedDelegate,
	                           int ActionExclusiveGroupID = 0);

	template <typename T>
	static T* CreateARPGAction(TSubclassOf<AARPGAction> ActionClass,
	                           AARPGCharacter* ActionOwnerCharacter, FTransform Transform,
	                           FActionFinishDelegate ActionFinishedDelegate, int ActionExclusiveGroupID = 0);

	virtual FActionFinishEvent& OnActionFinishedEvent()
	{
		return ActionFinishedEvent;
	}

	//- Getter&Setter -
	virtual int GetItemNumber() const override { return -1; };
	virtual FText GetItemNumberText() const override { return FText{}; };

	EActionStateEnum GetActionState() const { return ActionState; }
	void SetActionState(const EActionStateEnum NewActionState) { this->ActionState = NewActionState; }

	AARPGCharacter* GetUser() const { return User.Get(); }
	void SetUser(const TWeakObjectPtr<AARPGCharacter>& NewUser) { this->User = NewUser; }

	AARPGCharacter* GetTarget() const { return Target.Get(); }
	void SetTarget(const TWeakObjectPtr<AARPGCharacter>& NewTarget) { this->Target = NewTarget; }
	//- Getter&Setter -
};


UCLASS(Blueprintable)
class AARPGMontageAction : public AARPGAction
{
	GENERATED_BODY()

	UPROPERTY()
	UAnimInstance* OwnerCharacterAnimInstance;

	UPROPERTY(BlueprintReadWrite, Category="ARPGMontageAction", meta=(AllowPrivateAccess))
	UAnimMontage* ActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGMontageAction", meta=(AllowPrivateAccess))
	FName StartSectionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ARPGMontageAction", meta=(AllowPrivateAccess))
	float PlayRate = 1;

	int MontageInstanceID;

	void BindDelegateToOwnerCharacterAnimInstance();

protected:

	virtual void BeginPlay() override;

	virtual void OnActionActivate() override;
	virtual void OnActionFinished(AARPGAction* Action) override;

	UFUNCTION()
	void BindToMontageBegin(UAnimMontage* Montage);
	UFUNCTION()
	void BindToMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	UFUNCTION()
	void BindToMontageStop(UAnimMontage* Montage, bool bInterrupted);


	/*-Start- Play montage callback -Start-*/
	virtual void OnMontageBegin(UAnimMontage* Montage) { ; }
	virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) { ; }
	virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) { if (!bInterrupted) { FinishAction(); } }
	/*-End- Play montage callback -End-*/
	

	/*-Start- Blueprint play montage callback -Start-*/
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnMontageBegin")
    void BPFunc_OnMontageBegin(AARPGCharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnMontageNotify")
    void BPFunc_OnMontageNotify(FName NotifyName, AARPGCharacter* Character);
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnMontageStop")
    void BPFunc_OnMontageStop(AARPGCharacter* Character);
	/*-End- Blueprint play montage callback -End-*/
	
public:
	/*- Getter&Setter */
	UAnimMontage* GetActionMontage() const { return ActionMontage; }
	void SetActionMontage(UAnimMontage* const NewActionMontage) { this->ActionMontage = NewActionMontage; }
	/*- Getter&Setter */
};

USTRUCT(BlueprintType)
struct FMeleeAttackActionDescriptionStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGMeleeAttackCollection", meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> MeleeAttackMontages;
};

UCLASS(Blueprintable, HideCategories=("ARPGMontageAction"))
class AARPGMeleeAttackAction : public AARPGMontageAction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="ARPGMeleeAttackAction", meta=(AllowPrivateAccess))
	int MeleeAttackIndex;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGMeleeAttackCollection", meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> MeleeAttackMontages;

	virtual void OnActionActivate() override;

	virtual void OnMontageBegin(UAnimMontage* Montage) override;

	virtual void OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload) override;

	virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;
};

UCLASS(Blueprintable, HideCategories=("ARPGMontageAction"))
class AARPGMultiMontageAction : public AARPGMontageAction
{
	GENERATED_BODY()

	int MontageIndex;
	FTimerHandle ResetTimerHandle;
	float ResetDelay = 3;

protected:
	virtual void OnActionActivate() override;
	virtual void OnActionFinished(AARPGAction* Action) override;

	virtual void OnMontageStop(UAnimMontage* Montage, bool bInterrupted) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGMultiMontageAction", meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> ActionMontages;
};


UCLASS(Blueprintable)
class AARPGBuff : public AARPGAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPG Buff", meta=(AllowPrivateAccess))
	float BuffDuration = 30;
	FTimerHandle BuffTimerHandle;

	virtual void OnActionActivate() override;

public:
	void SetDuration(const float Duration)
	{
		BuffDuration = Duration;
		GetWorldTimerManager().SetTimer(BuffTimerHandle,
		                                FTimerDelegate::CreateLambda([&]() { FinishAction(); }),
		                                BuffDuration, false);
	}
};
