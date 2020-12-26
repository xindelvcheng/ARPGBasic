// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "ARPGCharacter.generated.h"

class UCharacterConfigPrimaryDataAsset;

UENUM()
enum class EActionResult:uint8
{
	ActionSuccess,
	ActionFail
};

static const FString LevelNames[] = {
	TEXT("练气"),
	TEXT("筑基"),
	TEXT("金丹"),
	TEXT("元婴"),
	TEXT("分神"),
	TEXT("合体"),
	TEXT("渡劫"),
	TEXT("大乘")
};

UENUM(BlueprintType)
enum class EGroundTypeEnum : uint8
{
	Concrete,
	Grass,
	Wood,
	Sand
};

USTRUCT(BlueprintType)
struct FGroundTypeFootstepSoundPairStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterFootstepSoundEffect")
	EGroundTypeEnum GroundType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterFootstepSoundEffect")
	USoundBase* FootstepSoundEffect;
};

static const FString EnvironmentNames[] = {
	TEXT("混凝土")
};

UCLASS()
class AARPGCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC",meta=(AllowPrivateAccess))
	UCharacterConfigPrimaryDataAsset* CharacterConfigPDataAsset;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGBASIC",meta=(AllowPrivateAccess))
	class UARPGGameItemsManagerComponent* GameItemsManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AimComponent, meta = (AllowPrivateAccess))
	class UARPGAimComponent* AimComponent;

public:
	// Sets default values for this character's properties
	AARPGCharacter();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPGBASIC",meta=(AllowPrivateAccess))
	FName CharacterName = "DefaultCharacter";
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
	class UCharacterStatusComponent* CharacterStatusComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
	class UTranscendentalCombatComponent* CharacterCombatComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
	class UARPGLockTargetComponent* CharacterLockTargetComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGCharacterBasicComponent")
	class UARPGAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSourceComponent;


	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere,Category="ARPGCharacter")
	TArray<UAnimMontage*> HitReactAnimMontages;

	int HitReactIndex = 0;

	//Tolerance
	TMap<TSubclassOf<UDamageType>,int> DamageToleranceCounter;

	FTimerHandle ResetHitReactIndexTimerHandle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource")
	UParticleSystem* ImpactVisualEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource")
	USoundCue* ImpactSoundEffect;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ARPG Art Resource")
	TArray<FGroundTypeFootstepSoundPairStruct> FootstepSoundEffects;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	FString GetLevelName() const
	{
		if (CharacterStatusComponent)
		{
			const int currentLevel = CharacterStatusComponent->GetLevel();
			TArray<FStringFormatArg> FormatArray = {LevelNames[currentLevel / 9], (currentLevel + 1) / 9};
			return FString::Format(TEXT("{0}{1}层"), std::move(FormatArray));
		}
		return FString();
	}

	//此函数在AnimInstance中调用，用于播放脚步声
	virtual void PlayFootStepSoundEffect(EGroundTypeEnum GroundType, float Volume = 1);

protected:
	UFUNCTION()
	void ReInitCharacterArtResources(UCharacterConfigPrimaryDataAsset* CharacterConfigDataAsset);

public:
	UFUNCTION(CallInEditor,Category="ARPGBASIC")
	void RefreshWithCharacterConfigPDataAsset();
	

	UCharacterStatusComponent* GetCharacterStatusComponent() const
	{
		return CharacterStatusComponent;
	}

	UARPGLockTargetComponent* GetCharacterLockTargetComponent() const
	{
		return CharacterLockTargetComponent;
	}

	UARPGGameItemsManagerComponent* GetGameItemsManagerComponent() const
	{
		return GameItemsManagerComponent;
	}


	virtual UARPGAimComponent* GetAimComponent() const
	{
		return AimComponent;
	}

	FText GetCharacterDisplayName() const;

	//   转发常用属性到CharacterStatusComponent

	UFUNCTION(BlueprintPure,Category="ARPGBASIC")
	int GetCharacterLevel() const
	{
		return CharacterStatusComponent->GetLevel();
	}

	UFUNCTION(BlueprintPure,Category="ARPGBASIC")
	int GetMaxHP() const
	{
		return CharacterStatusComponent->GetMaxHP();
	}


	UFUNCTION(BlueprintPure,Category="ARPGBASIC")
	int GetCurrentHP() const
	{
		return CharacterStatusComponent->GetCurrentHP();
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	void SetCurrentHP(const int CurrentHP) const
	{
		CharacterStatusComponent->SetCurrentHP(CurrentHP);
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	void UpdateCurrentHP(const int DeltaHP) const
	{
		CharacterStatusComponent->UpdateCharacterHP(DeltaHP);
	}

	UFUNCTION(BlueprintPure,Category="ARPGBASIC")
	int GetMaxSP() const
	{
		return CharacterStatusComponent->GetMaxSP();
	}

	UFUNCTION(BlueprintPure,Category="ARPGBASIC")
	int GetCurrentSP() const
	{
		return CharacterStatusComponent->GetCurrentSP();
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	void SetCurrentSP(const int CurrentSP) const
	{
		CharacterStatusComponent->SetCurrentSP(CurrentSP);
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	void UpdateCurrentSP(const int DeltaSP) const
	{
		CharacterStatusComponent->UpdateCharacterSP(DeltaSP);
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	int GetCoins() const
	{
		return CharacterStatusComponent->GetCoins();
	}

	UFUNCTION(BlueprintCallable,Category="ARPGBASIC")
	void UpdateCoins(const int DeltaCoins) const
	{
		CharacterStatusComponent->UpdateCharacterCoins(DeltaCoins);
	}

	FCharacterPropertyChangedEvent& OnCharacterPropertyChanged() const
	{
		return CharacterStatusComponent->OnCharacterPropertyChanged;
	}

	//转发常用属性到ARPGCharacterCombatComponent
	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	UTranscendentalCombatComponent* GetCharacterCombatComponent() const
	{
		return CharacterCombatComponent;
	}

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	bool TryToMeleeAttack()
	{
		return CharacterCombatComponent->TryToMeleeAttack();
	};


	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	bool TryToUseAbility(const int AbilityIndex)
	{
		return (CharacterCombatComponent->TryToUseAbility(AbilityIndex));
	};

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	bool CauseRigid(const float Duration, AARPGCharacter* Causer)
	{
		return CharacterCombatComponent->CauseRigid(Duration, Causer);
	};

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	bool GetIsRigid() const { return CharacterCombatComponent->GetIsRigid(); }

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	bool GetIsActing() const { return CharacterCombatComponent->GetIsActing(); }

	UFUNCTION(BlueprintCallable,Category="ARPGCharacterCombatComponent")
	AARPGAction* GetCurrentActiveAction() const { return CharacterCombatComponent->GetCurrentActiveAction(); }
};
