// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeDamageDetectAnimNotifyState.h"


#include "ARPGCharacter.h"
#include "ARPGDamageSubsystem.h"
#include "ARPGGameInstanceSubsystem.h"
#include "CharacterStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMeleeDamageDetectAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	if (AARPGCharacter* OwnerCharacter = Cast<AARPGCharacter>(MeshComp->GetOwner()))
	{
		if (UARPGDamageSubsystem::Get(OwnerCharacter->GetWorld()))
		{
			WeaponDamageBoxCollision = WeaponDamageBoxCollision
				                           ? Cast<UBoxComponent>(
					                           OwnerCharacter->GetComponentByClass(UBoxComponent::StaticClass()))
				                           : WeaponDamageBoxCollision;
			if (WeaponDamageBoxCollision)
			{
				FDamageDetectDescriptionStruct DetectDescription{
					true, DamageBoxHalfSizeInTrace, true, DamageWeight, DamageBias, VelocityDamageBonusWeight,
					DamageTypeClass
				};
				DamageDetectRecord = UARPGDamageSubsystem::Get(OwnerCharacter->GetWorld())->RegisterToDamageDetect(
					WeaponDamageBoxCollision, OwnerCharacter, FDamageDetectedDelegate{}, std::move(DetectDescription));
				DamageDetectRecord->bDrawDebug = true;
			}
			else
			{
				UARPGGameInstanceSubsystem::PrintLogToScreen(
					TEXT("错误，使用TestProject检测伤害的角色必须指定WeaponDamageBoxCollision(应为包裹武器的BoxCollision)"), 15, FColor::Red);
			}
		}
	}
}

void UMeleeDamageDetectAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	if (UARPGDamageSubsystem::Get(MeshComp->GetWorld()))
	{
		UARPGDamageSubsystem::Get(MeshComp->GetWorld())->UnRegisterToDamageDetect(DamageDetectRecord.Get());
	}
}
