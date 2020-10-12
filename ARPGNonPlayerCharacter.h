// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ARPGCharacter.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"

#include "ARPGNonPlayerCharacter.generated.h"

UCLASS()
class AARPGNonPlayerCharacter : public AARPGCharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AARPGNonPlayerCharacter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};

UCLASS()
class AARPGBossAI : public AARPGNonPlayerCharacter
{
    GENERATED_BODY()
};
