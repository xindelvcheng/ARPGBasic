// Fill out your copyright notice in the Description page of Project Settings.


#include "GameItem.h"

#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AGameItem::AGameItem()
{
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
    RootComponent = BoxCollision;

    PromptFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PromptFX"));
    PromptFX->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGameItem::BeginPlay()
{
    Super::BeginPlay();
    if (!PromptFX->Template || !ItemIcon)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, TEXT("道具未指定美术资源"));
    }
}

AGameItem* AGameItem::BeTaken()
{
    PromptFX->DestroyComponent();
    BoxCollision->DestroyComponent();
    IsInBag = true;
    return this;
}
