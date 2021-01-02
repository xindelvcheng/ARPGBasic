// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeDamageDetectAnimNotifyState.h"


#include "ARPGCharacter.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"
#include "CharacterStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMeleeDamageDetectAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	if (AARPGCharacter* OwnerCharacter = Cast<AARPGCharacter>(MeshComp->GetOwner()))
	{
		//如果没有指定WeaponDamageBoxCollision，取该角色的第一个UARPGDamageBoxComponent作为如果没有指定WeaponDamageBoxCollision
		if (!WeaponDamageBoxCollision)
		{
			WeaponDamageBoxCollision = Cast<UARPGDamageBoxComponent>(
                OwnerCharacter->GetComponentByClass(UARPGDamageBoxComponent::StaticClass()));
			if (!WeaponDamageBoxCollision)
			{
				UARPGStaticFunctions::PrintLogToScreen(
                    TEXT(
                        "错误，使用UMeleeDamageDetectAnimNotifyState检测伤害的角色必须指定WeaponDamageBoxCollision(应为包裹武器的UARPGDamageBoxComponent)"),
                    15, FColor::Red);
				return;
			}
		}
		FDamageDetectDescriptionStruct DetectDescription{
			true, DamageBoxHalfSizeInTrace, true, DamageWeight, DamageBias, VelocityDamageBonusWeight,
            DamageTypeClass
        };
		WeaponDamageBoxCollision->SetDetectDescription(std::move(DetectDescription));
		WeaponDamageBoxCollision->EnableDamageDetected();
	}
}

void UMeleeDamageDetectAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (WeaponDamageBoxCollision)
	{
		WeaponDamageBoxCollision->DisableDamageDetected();
	}
}
