// Fill out your copyright notice in the Description page of Project Settings.


#include "GameItem.h"

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
AGameItem::AGameItem()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    RootComponent = BoxCollision;

    PromptFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptFX"));
    PromptFX->SetupAttachment(RootComponent);
    if (UARPGBasicSettings::Get() && UARPGBasicSettings::Get()->DefaultGameItemPickUpPromptVisualEffect)
    {
        PromptFX->SetTemplate(UARPGBasicSettings::Get()->DefaultGameItemPickUpPromptVisualEffect.LoadSynchronous());
    }

    BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
    BillboardComponent->SetupAttachment(RootComponent);
    if (ItemIcon)
    {
        BillboardComponent->SetSprite(ItemIcon);
    }
}

// Called when the game starts or when spawned
void AGameItem::BeginPlay()
{
    Super::BeginPlay();
    if (!PromptFX->Template || !ItemIcon)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("道具未指定美术资源"));
    }

    if (EffectActionClass)
    {
        FActorSpawnParameters ActorSpawnParameters;
        ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        FTransform Transform;
        EffectAction = Cast<AARPGAction>(GetWorld()->SpawnActor(EffectActionClass, &Transform, ActorSpawnParameters));
    }
}

AGameItem* AGameItem::BeTaken(AARPGCharacter* Character)
{
    BoxCollision->DestroyComponent();
    RootComponent = PromptFX;
    IsInBag = true;
    OwnerCharacter = Character;
    FMoveFinishDelegate MoveFinishDelegate;
    MoveFinishDelegate.BindDynamic(this,&AGameItem::BindToBeTaken);
    UARPGGameInstanceSubsystem::MoveActorTowardActorWithScale(this,OwnerCharacter,MoveFinishDelegate);
    return this;
}

void AGameItem::BindToBeTaken()
{
    PromptFX->DestroyComponent();
}

void AGameItem::NativeUseGameItem(AARPGCharacter* User)
{
    if (EffectAction)
    {
        EffectAction->InitWithOwningCharacter(User);
        EffectAction->OnActionFinishedEvent().AddUObject(User->GetCharacterCombatComponent(),
                                                   &UARPGCharacterCombatComponent::BindToOnActionFinished);
    }

    BeUsed(User);
}
