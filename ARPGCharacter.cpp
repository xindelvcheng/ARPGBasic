// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGCharacter.h"

#include "ARPGConfigSubsystem.h"
#include "ARPGGameItemsManagerComponent.h"
#include "CharacterStatusComponent.h"

// Sets default values
AARPGCharacter::AARPGCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    CharacterStatusComponent = CreateDefaultSubobject<UCharacterStatusComponent>("CharacterStatusComponent");
    
}
 
// Called when the game starts or when spawned
void AARPGCharacter::BeginPlay()
{
    Super::BeginPlay();

    //检查角色是否设置了名称
    if (CharacterName.IsNone() || CharacterName.IsEqual("DefaultCharacter"))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("未指定角色CharacterName属性，ARPGCharacter无法进行初始化"));
        }
        
    }
    CharacterStatusComponent->SetCharacterName(CharacterName);

}

// Called every frame
void AARPGCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AARPGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
