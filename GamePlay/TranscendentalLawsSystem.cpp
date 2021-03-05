// Fill out your copyright notice in the Description page of Project Settings.


#include "TranscendentalLawsSystem.h"
#include "ARPGAction.h"
#include "ARPGCharacter.h"
#include "ARPGCharacterCombatComponent.h"
#include "ARPGCoreSubsystem.h"
#include "ARPGStaticFunctions.h"


FText ATranscendentalLawsSystem::GetTranscendentalLawsAttainmentText_Implementation()
{
	switch (Attainment)
	{
	case ETranscendentalLawsAttainment::Low:
		return FText::FromString(TEXT("入门"));
	case ETranscendentalLawsAttainment::Middle:
		return FText::FromString(TEXT("小成"));
	case ETranscendentalLawsAttainment::High:
		return FText::FromString(TEXT("大成"));
	case ETranscendentalLawsAttainment::Perfect:
		return FText::FromString(TEXT("极致"));
	}
	return FText::FromString(TEXT("无"));
}

void ATranscendentalLawsSystem::SetAttachedCharacter(AARPGCharacter* NewAttachedCharacter)
{
	AttachedCharacter = NewAttachedCharacter;
	AttachedCharacterCombatComponent = AttachedCharacter->GetCharacterCombatComponent();
	checkf(AttachedCharacter.IsValid()&&AttachedCharacterCombatComponent.IsValid(),
	       TEXT("错误，角色不含有CharacterCombatComponent"))
}

void ATranscendentalLawsSystem::SetCurrentState(ETranscendentalLawsAttainment NewState)
{
	Attainment = NewState;
}

void ATranscendentalLawsSystem::Init(AARPGCharacter* NewAttachedCharacter,
                                     UARPGCharacterCombatComponent* NewAttachedCharacterCombatComponent)
{
	AttachedCharacter = NewAttachedCharacter;
	AttachedCharacterCombatComponent = NewAttachedCharacterCombatComponent;


	SpawnActionActors(MeleeAttackClasses, MeleeAttackCollectionActions);
	SpawnActionActors(RemoteAttackClasses, RemoteAttackActions);
	SpawnActionActors(AbilityClasses, AbilityActions);
	SpawnActionActors(BuffClasses, BuffActions);
}

void ATranscendentalLawsSystem::Activate()
{
	// checkf(AttachedCharacter.IsValid()&&AttachedCharacterCombatComponent.IsValid(), TEXT("错误，激活仙道时未指定用户"))
	//
	// if (MeleeAttackCollectionActions.IsValidIndex(0))
	// {
	// 	AttachedCharacterCombatComponent->MeleeAttackCollectionActions.Emplace(MeleeAttackCollectionActions[0]);
	// 	AttachedCharacterCombatComponent->CurrentMeleeAttackCollection = MeleeAttackCollectionActions[0];
	// }
}

void ATranscendentalLawsSystem::DeActivate()
{
	// AttachedCharacterCombatComponent->MeleeAttackCollectionActions.RemoveAll([&](const AARPGAction* Action)-> bool
	// {
	// 	return MeleeAttackCollectionActions.Contains(Action);
	// });
	// AttachedCharacterCombatComponent->CurrentMeleeAttackCollection = AttachedCharacterCombatComponent->
	// 	MeleeAttackCollectionActions[0];
}

void ATranscendentalLawsSystem::BeginPlay()
{
	Super::BeginPlay();
}

void ATranscendentalLawsSystem::SpawnActionActors(const TArray<TSubclassOf<AARPGAction>>& ActionClasses,
                                                  TArray<AARPGAction*>& ActionActors)
{
	for (const auto ActionClass : ActionClasses)
	{
		if (ActionClass)
		{
			AARPGAction* Action = AARPGAction::CreateARPGAction<AARPGAction>(
            ActionClass, AttachedCharacter.Get(), FActionFinishDelegate::CreateUObject(
                AttachedCharacterCombatComponent.Get(),
                &UARPGCharacterCombatComponent::BindToActionFinished));
			ActionActors.Add(Action);
		}else
		{
			UARPGStaticFunctions::PrintLogToScreen(FString::Printf(TEXT("%s ATranscendentalLawsSystem的Action存在NULL"),*AttachedCharacter->GetName()));
		}
	}
}