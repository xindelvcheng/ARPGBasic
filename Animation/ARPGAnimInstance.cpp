// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGAnimInstance.h"


#include "ARPGCharacter.h"
#include "Kismet/GameplayStatics.h"

void UARPGAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

bool UARPGAnimInstance::HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	auto GetPawnOwner = TryGetPawnOwner();
	if (const auto OwningCharacter = Cast<AARPGCharacter>(TryGetPawnOwner()))
	{
		const FString NotifyName = AnimNotifyEvent.NotifyName.ToString();

		if (NotifyName == TEXT("Footstep") || NotifyName == TEXT("FootStep"))
		{
			OwningCharacter->PlayFootStepSoundEffect(EGroundTypeEnum::Concrete);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimNotify：%s未处理，已跳过"), *NotifyName);
		}
	}

	return false;
}
