// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGActorComponent.h"
#include "Components/ActorComponent.h"
#include "ARPGCharacterStatusComponent.generated.h"

/**
 * 此类相当于C#中的属性，可以直接以public暴露
 */
template <class T = float>
class TCharacterProperty
{
	T Value;
	T MaxValue;
	int Specialty = 0;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FPropertyChangeEvent, T, T, int);
	FPropertyChangeEvent ChangedDelegate;

public:
	TCharacterProperty() = default;

	//用于无上限的属性（最大值将被设置成FLT_MAX）
	TCharacterProperty(T value)
	{
		this->Value = value;
		this->MaxValue = FLT_MAX;
	}

	//有上限的属性，其=、+=、-=运算都不会超过最大值
	TCharacterProperty(T value, T maxValue)
	{
		this->Value = value;
		this->MaxValue = maxValue;
	}

	void operator=(const T& value)
	{
		this->Value = value < MaxValue ? value : MaxValue;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void operator+=(const T& deltaValue)
	{
		this->Value = Value + deltaValue < 0 ? 0 : Value + deltaValue > MaxValue ? MaxValue : Value + deltaValue;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void operator-=(const T& deltaValue)
	{
		this->Value = Value - deltaValue < 0 ? 0 : Value - deltaValue > MaxValue ? MaxValue : Value - deltaValue;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void operator*=(const T& alpha)
	{
		this->Value = Value * alpha < 0 ? 0 : Value * alpha > MaxValue ? MaxValue : Value * alpha;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void operator/=(const T& alpha)
	{
		this->Value = Value / alpha < 0 ? 0 : Value / alpha > MaxValue ? MaxValue : Value / alpha;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}


	T GetValue() { return Value; }
	T GetMaxValue() { return MaxValue; }
	int GetSpecialty() const { return Specialty; }

	void SetValue(const T& value)
	{
		this->Value = value < MaxValue ? value : MaxValue;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void SetMaxValue(const T& maxValue)
	{
		this->MaxValue = maxValue;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void AddSpecialty()
	{
		Specialty++;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	void SetSpecialty(const int NewSpecialty)
	{
		Specialty = NewSpecialty;
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}

	operator T()
	{
		return Value;
	}

	FPropertyChangeEvent& OnChanged() { return ChangedDelegate; }

	void BindLambdaToOnChanged(const std::function<void (float Value, float MaxValue, int Specialty)>& Lambda)
	{
		ChangedDelegate.AddLambda(Lambda);
		ChangedDelegate.Broadcast(Value, MaxValue, Specialty);
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTPROJECT_API UARPGCharacterStatusComponent : public UARPGActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UARPGCharacterStatusComponent();

	TCharacterProperty<float> HP;
	TCharacterProperty<float> MP;

protected:
	virtual void InitializeComponent() override;
	// Called when the game starts
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
