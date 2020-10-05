// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatusComponent.h"


#include "ArchiveStructs.h"
#include "ARPGBasicSettings.h"

void UCharacterStatusComponent::SetCharacterName(const FName& NewCharacterName)
{
    this->CharacterName = NewCharacterName;
    ReInitCharacterProperties(true);
}

void UCharacterStatusComponent::ReInitCharacterProperties(bool bInitFromDataTable)
{
    if (bInitFromDataTable)
    {
        const auto CharactersConfigDataTable = LoadObject<UDataTable>(
        nullptr, *UARPGBasicSettings::Get()->CharactersConfig.ToString());
        if (!CharactersConfigDataTable)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("无效的配置文件"));
            }
            return;
        }
        FCharacterInitConfigStruct* CharacterArchiveStruct = CharactersConfigDataTable->FindRow<FCharacterInitConfigStruct>(
            CharacterName,TEXT("PostEditChangeProperty"));
        if (CharacterArchiveStruct)
        {
            Level = CharacterArchiveStruct->Level;
            SetCoins(CharacterArchiveStruct->Coins);
        
            HealthSpecialty = CharacterArchiveStruct->HealthSpecialty;
            StaminaSpecialty = CharacterArchiveStruct->StaminaSpecialty;
            AttackSpecialty = CharacterArchiveStruct->AttackSpecialty;
            DefenseSpecialty = CharacterArchiveStruct->DefenseSpecialty;
            ToughnessSpecialty = CharacterArchiveStruct->ToughnessSpecialty;
        }
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



// Called when the game starts
void UCharacterStatusComponent::BeginPlay()
{
    Super::BeginPlay();

    if (CharacterName == "DefaultCharacter")
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow,TEXT("角色未设定CharacterName"));
        }
    }
    ReInitCharacterProperties(true);
}
