// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGAction.h"
#include "ARPGActorComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"


#include "ARPGCharacterCombatComponent.generated.h"

class UCharacterConfigPrimaryDataAsset;
class AARPGAction;
class AARPGCastAction;
class AARPGCharacter;
DECLARE_MULTICAST_DELEGATE(FARPGMeleeAttackActionDelegate);

UCLASS(ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent))
class UARPGCharacterCombatComponent : public UARPGActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<int, AARPGAction*> ExclusiveGroupActionsMap;

	/*技能配置，推荐在角色的CharacterConfigPDataAsset中统一配置（bool bUseCharacterDataAssetInit = true中时此处配置将被覆盖）*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess))
	bool bUseCharacterDataAssetInit = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess,
		EditCondition="!bUseCharacterDataAssetInit", EditConditionHides))
	TArray<FMeleeAttackActionDescriptionStruct> MeleeAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess,
		EditCondition="!bUseCharacterDataAssetInit", EditConditionHides))
	bool AllowInterruptBackswing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess,
		EditCondition="!bUseCharacterDataAssetInit", EditConditionHides))
	TArray<TSubclassOf<AARPGCastAction>> AbilityClasses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess,
		EditCondition="!bUseCharacterDataAssetInit", EditConditionHides))
	TArray<FName> SpellNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ARPGCharacterCombatComponent", meta=(AllowPrivateAccess,
		EditCondition="!bUseCharacterDataAssetInit", EditConditionHides))
	TArray<TSubclassOf<AARPGBuff>> BuffClasses;


	/*缓存技能释放对象*/
	UPROPERTY()
	TArray<AARPGAction*> MeleeAttackCollectionActions;

	UPROPERTY()
	AARPGAction* CurrentMeleeAttackCollection;

	UPROPERTY()
	TArray<AARPGCastAction*> AbilityActions;

	

	UPROPERTY()
	TArray<AARPGBuff*> BuffActions;

	FARPGMeleeAttackActionDelegate MeleeAttackStartDelegate;
	FARPGMeleeAttackActionDelegate MeleeAttackEndDelegate;

public:
	// Sets default values for this component's properties
	UARPGCharacterCombatComponent();
	
	UPROPERTY()
	TMap<FName, AARPGCastAction*> AbilityActionsMap;

protected:
	virtual void InitializeComponent() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	bool IsRigid;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void BindToActionFinished(AARPGAction* Action);

	UFUNCTION(BlueprintCallable, Category="ARPGCharacterCombatComponent")
	bool GetIsRigid() const { return IsRigid; }

	UFUNCTION(BlueprintCallable, Category="ARPGCharacterCombatComponent")
	bool GetIsActing() const { return CurrentActiveAction ? true : false; }

	UFUNCTION(BlueprintCallable, Category="ARPGCharacterCombatComponent")
	AARPGAction* GetCurrentActiveAction() const { return CurrentActiveAction; }

	UPROPERTY(BlueprintReadOnly, Category="ARPGCharacterCombatComponent")
	AARPGAction* CurrentActiveAction;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatEvent);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ARPGCharacterCombatComponent")
	FCombatEvent ActionStart;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category="ARPGCharacterCombatComponent")
	FCombatEvent ActionEnd;

	virtual bool TryToMeleeAttack();

	UFUNCTION()
	virtual bool TryToCastSpell(AARPGCastAction* Spell);

	UFUNCTION()
	virtual bool CauseRigid(float Duration, AARPGCharacter* Causer);

	UFUNCTION()
	virtual bool ActivateBuff(int BuffIndex, float Duration, AARPGCharacter* Instigator = nullptr);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRigidEvent, float, Duration);

	UPROPERTY(BlueprintAssignable, Category="ARPGCharacterCombatComponent")
	FRigidEvent OnRigid;
	FRigidEvent OnResumeFromRigid;

	FTimerHandle RigidTimerHandle;
	FTimerDelegate RigidTimerDelegate;

	bool IsMoving;

	UFUNCTION(BlueprintCallable, Category="ARPGCharacterCombatComponent")
	void ReInitCharacterActions();


	virtual const TArray<FName>& GetSpellNames() const
	{
		return SpellNames;
	}

	virtual void SetSpellNames(const TArray<FName>& NewSpellNames)
	{
		this->SpellNames = NewSpellNames;
		ReInitCharacterActions();
	}

	virtual void AppendSpellNames(FName NewSpellName)
	{
		this->SpellNames.Emplace(NewSpellName);
		ReInitCharacterActions();
	}


	virtual AARPGAction* GetCurrentMeleeAttackCollection()
	{
		return CurrentMeleeAttackCollection;
	}

	TArray<AARPGCastAction*> GetAbilityActions() const { return AbilityActions; };

	FARPGMeleeAttackActionDelegate& OnAttackStart() { return MeleeAttackStartDelegate; }

	FARPGMeleeAttackActionDelegate& OnAttackEnd() { return MeleeAttackEndDelegate; }
};

UCLASS()
class UActivateActionBlueprintNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionFinished);

	UPROPERTY(BlueprintAssignable)
	FActionFinished ActionFinished;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UActivateActionBlueprintNode* ActivateAction(AARPGCharacter* Instigator, FName ActionName, bool& bActivateSuccess);

private:
	bool ActivateActionInternal(AARPGCharacter* Instigator, FName ActionName);
};
