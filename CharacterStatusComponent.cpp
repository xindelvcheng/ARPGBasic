// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusComponent.h"


#include "ArchiveStructs.h"
#include "ARPGBasicSettings.h"
#include "CharacterConfigPrimaryDataAsset.h"

void UCharacterStatusComponent::ReInitCharacterProperties(UCharacterConfigPrimaryDataAsset* CharacterConfigDataAsset)
{
    if (CharacterConfigDataAsset)
    {
        Level = CharacterConfigDataAsset->Level;
        SetCoins(CharacterConfigDataAsset->Coins);
        
        HealthSpecialty = CharacterConfigDataAsset->HealthSpecialty;
        StaminaSpecialty = CharacterConfigDataAsset->StaminaSpecialty;
        AttackSpecialty = CharacterConfigDataAsset->AttackSpecialty;
        DefenseSpecialty = CharacterConfigDataAsset->DefenseSpecialty;
        ToughnessSpecialty = CharacterConfigDataAsset->ToughnessSpecialty;
    }
    
    SetMaxHP(CalculateValueBySpecialty(HealthSpecialty));
    SetCurrentHP(MaxHP);
    SetMaxSP(CalculateValueBySpecialty(StaminaSpecialty));
    SetCurrentSP(MaxSP);

    Attack = CalculateValueBySpecialty(AttackSpecialty);
    Defense = CalculateValueBySpecialty(DefenseSpecialty);
    Toughness = CalculateValueBySpecialty(ToughnessSpecialty);
}




// Sets default values for this component's properties
UCharacterStatusComponent::UCharacterStatusComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

bool UCharacterStatusComponent::LevelUp(const ESpecialties Specialty)
{
    const float LevelUpCost = GetLevelUpCost();
    if (Coins >= LevelUpCost)
    {
        UpdateCharacterCoins(-LevelUpCost);

        switch (Specialty)
        {
        case ESpecialties::Health:
            HealthSpecialty++;
            break;
        case ESpecialties::Stamina:
            StaminaSpecialty++;
            break;
        case ESpecialties::Attack:
            AttackSpecialty ++;
            break;
        case ESpecialties::Defense:
            DefenseSpecialty ++;
            break;
        case ESpecialties::Toughness:
            ToughnessSpecialty ++;
        default:
            if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("无效的升级加点")); }
        }
        SetLevel(Level+1);
        return true;
    }
    return false;
}