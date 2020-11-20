// Fill out your copyright notice in the Description page of Project Settings.


#include "GameItem.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Particles/ParticleSystemComponent.h"
#include "ARPGAction.h"
#include "ARPGBasicSettings.h"
#include "ARPGCharacter.h"
#include "Components/BillboardComponent.h"

// Sets default values
AGameItem::AGameItem()
{
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

AGameItem* AGameItem::BeTaken()
{
    PromptFX->DestroyComponent();
    BoxCollision->DestroyComponent();
    IsInBag = true;
    return this;
}

void AGameItem::NativeUseGameItem(AARPGCharacter* User)
{
    if (EffectAction)
    {
        EffectAction->InitWithOwningCharacter(User);
        EffectAction->OnActionFinished.BindUObject(User->GetCharacterCombatComponent(),
                                                   &UARPGCharacterCombatComponent::BindToOnActionFinished);
    }

    BeUsed(User);
}
