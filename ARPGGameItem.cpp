// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGameItem.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Particles/ParticleSystemComponent.h"
#include "ARPGAction.h"
#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"
#include "Components/BillboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AARPGGameItem::AARPGGameItem()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;

	PromptFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptFX"));
	PromptFX->SetupAttachment(RootComponent);
	if (UARPGBasicSettings* BasicSettings = UARPGBasicSettings::Get())
	{
		if (!PromptFX->Template)
		{
			PromptFX->SetTemplate(BasicSettings->DefaultGameItemPickUpPromptVisualEffect.LoadSynchronous());
		}
	}
	
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	BillboardComponent->SetupAttachment(RootComponent);
	if (!BillboardComponent->Sprite)
	{
		BillboardComponent->SetSprite(GetItemIcon());
	}
}

// Called when the game starts or when spawned
void AARPGGameItem::BeginPlay()
{
	Super::BeginPlay();
	if (!PromptFX->Template || !GetItemIcon())
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(FString::Printf(TEXT("%s道具未指定美术资源"), *GetName()));
	}
}

void AARPGGameItem::OnBeTaken(AARPGCharacter* Character)
{
}

AARPGGameItem* AARPGGameItem::PickUpGameItem(AARPGCharacter* Character)
{
	BoxCollision->DestroyComponent();
	RootComponent = PromptFX;
	IsInBag = true;
	SetOwnerCharacter(Character);

	FMoveFinishDelegate MoveFinishDelegate;
	MoveFinishDelegate.BindDynamic(this, &AARPGGameItem::BindToHasBeenTaken);
	UARPGGameInstanceSubsystem::MoveActorTowardActorWithScale(this, GetOwnerCharacter(), MoveFinishDelegate);

	OnBeTaken(Character);
	return this;
}

void AARPGGameItem::BindToHasBeenTaken()
{
	PromptFX->DestroyComponent();
	PromptFX = nullptr;
}

void AARPGGameItem::OnBeUsed(AARPGCharacter* User)
{
	
}

bool AARPGGameItem::TryToUseGameItem(AARPGCharacter* User)
{
	if (GetItemNumber() > 0)
	{
		OnBeUsed(User);
		BPFunc_OnBeUsed(User);
		return true;
	}
	return false;
}
