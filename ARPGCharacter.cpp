// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacter.h"

#include "ARPGConfigSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "ARPGLockTargetComponent.h"

AARPGCharacter::AARPGCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    CharacterStatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>("CharacterStatusComponent");
    CharacterCombatComponent = CreateDefaultSubobject<UTranscendentalCombatComponent>("ARPGCharacterCombaComponent");
    CharacterLockTargetComponent = CreateDefaultSubobject<UARPGLockTargetComponent>("ARPGLockTargetComponent");
}
 
void AARPGCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (CharacterName.IsNone() || CharacterName.IsEqual("DefaultCharacter"))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("未指定角色CharacterName属性，ARPGCharacter无法进行初始化"));
        }
        
    }
    CharacterStatusComponent->SetCharacterName(CharacterName);

}

void AARPGCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
