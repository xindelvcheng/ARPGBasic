// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeDamageDetectAnimNotifyState.h"


#include "ARPGCharacter.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"
#include "CharacterStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#pragma optimize("",off)
void UMeleeDamageDetectAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	OwnerCharacter = MeshComp->GetOwner<AARPGCharacter>();

	if (OwnerCharacter)
	{
		FindDamageBoxesByNames();
	}

	FDamageDetectDescriptionStruct DetectDescription{
		true, DamageWeight, DamageBias, VelocityDamageBonusWeight,
		DamageTypeClass
	};

	for (UARPGDamageBoxComponent* DamageBox : DamageBoxes)
	{
		if (DamageBox)
		{
			DamageBox->SetDetectDescription(std::move(DetectDescription));
			DamageBox->EnableDamageDetected();
		}
	}
}
#pragma optimize("",on)

void UMeleeDamageDetectAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	for (UARPGDamageBoxComponent* DamageBox : DamageBoxes)
	{
		if (DamageBox)
		{
			DamageBox->DisableDamageDetected();
		}
	}
}

void UMeleeDamageDetectAnimNotifyState::FindDamageBoxesByNames()
{
	TArray<UARPGDamageBoxComponent*> DamageBoxCandidates;
	OwnerCharacter->GetComponents(DamageBoxCandidates);

	if (DamageBoxCandidates.Num() == 0)
	{
		UARPGStaticFunctions::PrintLogToScreen(
			TEXT(
				"错误，使用UMeleeDamageDetectAnimNotifyState检测伤害的角色必须指定WeaponDamageBoxCollision(应为包裹武器的UARPGDamageBoxComponent)"),
			15, FColor::Red);
	}

	for (FName DamageBoxName : DamageBoxNames)
	{
		DamageBoxes.Add(*DamageBoxCandidates.FindByPredicate(
			[DamageBoxName](UARPGDamageBoxComponent* DamageBoxCandidate)
			{
				return DamageBoxCandidate ? DamageBoxCandidate->GetFName() == DamageBoxName : false;
			}));
	}
}
