// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "math.h"
#include "Kismet/KismetStringLibrary.h"


#include "CharacterStatusComponent.generated.h"

class UCharacterConfigPrimaryDataAsset;

template <class T = float>
class TCharacterProperty
{
    T Value;

    T MaxValue;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FPropertyChangeEvent, T, T);
    FPropertyChangeEvent ChangedDelegate;

public:
    //用于无上限的属性（最大值将被设置成INT_MAX）
    TCharacterProperty(T value)
    {
        this->Value = value;
        this->MaxValue = INT_MAX;
    }

    //有上限的属性，其=、+=、-=运算都不会超过最大值
    TCharacterProperty(T value, T maxValue)
    {
        this->Value = value;
        this->MaxValue = maxValue;
    }

    void SetMaxValue(const T& maxValue)
    {
        this->MaxValue = maxValue;
        ChangedDelegate.Broadcast(Value, MaxValue);
    }

    void operator=(const T& value)
    {
        this->Value = value <= MaxValue ? value : MaxValue;
        ChangedDelegate.Broadcast(Value, MaxValue);
    }

    void operator+=(const T& deltaValue)
    {
        this->Value = Value + deltaValue <= MaxValue ? Value + deltaValue : MaxValue;
        ChangedDelegate.Broadcast(Value, MaxValue);
    }

    void operator-=(const T& deltaValue)
    {
        this->Value = Value - deltaValue >= 0 ? Value + deltaValue : 0;
        ChangedDelegate.Broadcast(Value, MaxValue);
    }

    operator T()
    {
        return Value;
    }

    FPropertyChangeEvent& OnChanged() { return ChangedDelegate; }
};

UENUM(BlueprintType)
enum class ESpecialties:uint8
{
    Health,
    Stamina,
    Attack,
    Defense,
    Toughness
};


UENUM(BlueprintType)
enum class ECharacterState:uint8
{
    Normal,
    Rigid,
    Poisoning,
    Invulnerable
};

UENUM(BlueprintType)
enum class ECharacterProperty:uint8
{
    Level,
    MaxHP,
    CurrentHP,
    HealthSpecialty,
    MaxSP,
    CurrentSP,
    Coins,
    StaminaSpecialty,
    CharacterState,
    Attack,
    AttackSpecialty,
    Defense,
    DefenseSpecialty,
    Toughness,
    ToughnessSpecialty
};


// 定义使用的常数
const float BaseRigidDuration = 1.f;
const float SpecialtyFactor = 30.f;
const float SPCostPerNormalAttackFactor = 0.25;
const float SPResumeTime = 1.f;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FCharacterPropertyChangedEvent, ECharacterProperty,
                                              ChangedProperty, int, CurrentValue, int, TotalValue,
                                              int, DeltaValue);

UCLASS( ClassGroup=(ARPGBasic), meta=(BlueprintSpawnableComponent) )
class UCharacterStatusComponent : public UActorComponent

{
    GENERATED_BODY()

    FTimerHandle SPResumeTimerHandle;
    FTimerHandle SPGrowingTimerHandle;
    FTimerDelegate SPResumeTimerDelegate;
    FTimerDelegate SPGrowingTimerDelegate;

public:

    UPROPERTY(BlueprintAssignable)
    FCharacterPropertyChangedEvent OnCharacterPropertyChanged;

private:
    UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="CharacterStatusComponent",meta=(AllowPrivateAccess))
    FName CharacterName = "DefaultCharacter";

    int Level = 10;

    int MaxHP = 2.5;
    int CurrentHP = 2.5;
    int HealthSpecialty = 10;

    int MaxSP = 1;
    int CurrentSP = 1;
    int StaminaSpecialty = 0;

    int Attack = 1;
    int AttackSpecialty = 0;

    int Defense = 1;
    int DefenseSpecialty = 0;

    int Toughness = 1;
    int ToughnessSpecialty = 0;

    int Coins = 10;

    ECharacterState CharacterState = ECharacterState::Normal;

    TSubclassOf<UDamageType> MeleeDamageType;

public:
    void ReInitCharacterProperties(UCharacterConfigPrimaryDataAsset* CharacterConfigDataAsset = nullptr);

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetLevel() const { return Level; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetLevel(const int NewLevel)
    {
        const int Temp = Level;
        Level = NewLevel;
        ReInitCharacterProperties();
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::Level, Level, Level, Level - Temp);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetMaxHP() const { return MaxHP; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetMaxHP(const int NewMaxHP)
    {
        const int Temp = MaxHP;
        MaxHP = NewMaxHP;
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::MaxHP, MaxHP, MaxHP, MaxHP - Temp);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetCurrentHP() const { return CurrentHP; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetCurrentHP(const int NewCurrentHP)
    {
        const int Temp = CurrentHP;
        CurrentHP = FMath::Clamp(NewCurrentHP, 0, MaxHP);
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentHP, CurrentHP, MaxHP, CurrentHP - Temp);
        if (this->CurrentHP <= 0)
        {
            OnCharacterDeath.Broadcast();
        }
    }

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeathEvent);

    UPROPERTY(BlueprintAssignable)
    FCharacterDeathEvent OnCharacterDeath;

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void UpdateCharacterHP(const int DeltaHP)
    {
        //Check value in set
        SetCurrentHP(CurrentHP + DeltaHP);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetMaxSP() const { return MaxSP; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetMaxSP(const int NewMaxSP)
    {
        const int Temp = MaxSP;
        MaxSP = NewMaxSP;
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::MaxSP, MaxSP, MaxSP, MaxSP - Temp);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetCurrentSP() const { return CurrentSP; }

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterStaminaExhaustedEvent);

    UPROPERTY(BlueprintAssignable)
    FCharacterStaminaExhaustedEvent OnCharacterStaminaExhausted;

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetCurrentSP(const int NewCurrentSP)
    {
        const int Temp = CurrentSP;
        CurrentSP = FMath::Clamp(NewCurrentSP, 0, MaxSP);
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentSP, CurrentSP, MaxSP, CurrentSP - Temp);
        if (this->CurrentSP <= 0)
        {
            OnCharacterStaminaExhausted.Broadcast();
        }

        GetWorld()->GetTimerManager().ClearTimer(SPResumeTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(SPGrowingTimerHandle);

        if (!SPResumeTimerDelegate.IsBound())
        {
            SPResumeTimerDelegate.BindLambda([&]()
            {
                GetWorld()->GetTimerManager().SetTimer(SPGrowingTimerHandle, SPGrowingTimerDelegate, 0.1, true);
            });
        }

        if (!SPGrowingTimerDelegate.IsBound())
        {
            SPGrowingTimerDelegate.BindLambda([&]()
            {
                if (CurrentSP >= MaxSP)
                {
                    GetWorld()->GetTimerManager().ClearTimer(SPGrowingTimerHandle);
                    return;
                }
                const float FloatMaxSP = static_cast<float>(MaxSP);
                CurrentSP += static_cast<int>((FloatMaxSP / 20.f) * StaminaSpecialty) > 0
                                 ? static_cast<int>((FloatMaxSP / 100.f) * StaminaSpecialty)
                                 : 1;
                CurrentSP = CurrentSP > MaxSP ? MaxSP : CurrentSP;
                OnCharacterPropertyChanged.Broadcast(ECharacterProperty::CurrentSP, CurrentSP, MaxSP, CurrentSP - Temp);
            });
        }

        if (CurrentSP < MaxSP)
        {
            GetWorld()->GetTimerManager().SetTimer(SPResumeTimerHandle, SPResumeTimerDelegate, 3, false);
        }
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void UpdateCharacterSP(const int DeltaSP)
    {
        SetCurrentSP(CurrentSP + DeltaSP);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

    //TODO: 添加角色状态事件
    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetCharacterState(const ECharacterState& NewCharacterState)
    {
        const ECharacterState Temp = CharacterState;
        CharacterState = NewCharacterState;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE FName GetCharacterName() const { return CharacterName; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetCoins() const { return Coins; }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetCoins(const int NewCoins)
    {
        const int Temp = Coins;
        Coins = NewCoins;
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::Coins, Coins, Coins, Coins - Temp);
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void UpdateCharacterCoins(const int DeltaCoins)
    {
        const float Temp = Coins;
        Coins += DeltaCoins;
        OnCharacterPropertyChanged.Broadcast(ECharacterProperty::Coins, Coins, Coins, Coins - Temp);
    }

    // Sets default values for this component's properties
    UCharacterStatusComponent();

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int CalculateValueBySpecialty(const int Specialty) const
    {
        return pow(1.414, Level) * (1 + 0.1 * Specialty);
    }


    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    bool LevelUp(const ESpecialties Specialty);

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    FORCEINLINE int GetLevelUpCost() const { return FMath::Pow(1.1, Level) * (10 + 0.1 * Level); };


    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void ModifyAttack(float Delta = 0, float Radio = 1)
    {
        this->Attack = (Attack + Delta) * Radio;
    }

    UFUNCTION(BlueprintCallable,DisplayName="BeAttackedCalculateBaseDamage",Category="CharacterStatusComponent")
    int CalculateBaseDamage(float BaseDamage)
    {
        return BaseDamage * BaseDamage / (BaseDamage + Defense);
    }

protected:


public:

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitActorEvent, FHitResult, HitResult);

    UPROPERTY(BlueprintAssignable)
    FHitActorEvent OnAttackHitActor;

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetHealthSpecialty() const
    {
        return HealthSpecialty;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetStaminaSpecialty() const
    {
        return StaminaSpecialty;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetAttackSpecialty() const
    {
        return AttackSpecialty;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetDefenseSpecialty() const
    {
        return DefenseSpecialty;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetToughnessSpecialty() const
    {
        return ToughnessSpecialty;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetAttack() const
    {
        return Attack;
    }


    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetDefense() const
    {
        return Defense;
    }


    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    int GetToughness() const
    {
        return Toughness;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetToughness(const int NewToughness)
    {
        this->Toughness = NewToughness;
    }


    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    TSubclassOf<UDamageType> GetMeleeDamageType() const
    {
        return MeleeDamageType;
    }

    UFUNCTION(BlueprintCallable,Category="CharacterStatusComponent")
    void SetMeleeDamageType(const TSubclassOf<UDamageType>& NewMeleeDamageType)
    {
        this->MeleeDamageType = NewMeleeDamageType;
    }


    void SetHealthSpecialty(const int NewHealthSpecialty)
    {
        this->HealthSpecialty = NewHealthSpecialty;
    }

    void SetStaminaSpecialty(const int NewStaminaSpecialty)
    {
        this->StaminaSpecialty = NewStaminaSpecialty;
    }

    void SetAttackSpecialty(const int NewAttackSpecialty)
    {
        this->AttackSpecialty = NewAttackSpecialty;
    }

    void SetDefenseSpecialty(const int NewDefenseSpecialty)
    {
        this->DefenseSpecialty = NewDefenseSpecialty;
    }

    void SetToughnessSpecialty(const int NewToughnessSpecialty)
    {
        this->ToughnessSpecialty = NewToughnessSpecialty;
    }
};
