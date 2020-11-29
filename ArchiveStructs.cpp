#include "ArchiveStructs.h"


#include "ARPGCharacter.h"
#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"

FCharacterArchiveStruct FCharacterArchiveStruct::MakeArchiveStruct(AARPGCharacter* Character)
{
	if (!Character || !Character->GetCharacterStatusComponent()|| !Character->GetCharacterCombatComponent())
	{
		return {};
	}
	UCharacterStatusComponent* CharacterStatusComponent = Character->GetCharacterStatusComponent();
	UTranscendentalCombatComponent* CharacterCombatComponent = Character->GetCharacterCombatComponent();
	TArray<TSoftClassPtr<ATranscendentalLawsSystem>> TranscendentalLawsSystemClasses;
	for (ATranscendentalLawsSystem* TranscendentalLawsSystem : CharacterCombatComponent->
         GetAllTranscendentalLawsSystems())
	{
		TranscendentalLawsSystemClasses.Emplace(TranscendentalLawsSystem->GetClass()->GetPathName());
	}

	FCharacterArchiveStruct CharacterArchiveStruct = {
		Character->GetClass(),
        Character->GetTransform(),
        Character->CharacterName,
        Character->GetCharacterDisplayName(),
        CharacterStatusComponent->GetLevel(),
        CharacterStatusComponent->GetCoins(),
        CharacterStatusComponent->GetHealthSpecialty(),
        CharacterStatusComponent->GetStaminaSpecialty(),
        CharacterStatusComponent->GetAttackSpecialty(),
        CharacterStatusComponent->GetDefenseSpecialty(),
        CharacterStatusComponent->GetToughnessSpecialty(),
        CharacterStatusComponent->GetCurrentHP(),
        CharacterStatusComponent->GetCurrentSP(),
        CharacterStatusComponent->GetCharacterState(),
        std::move(TranscendentalLawsSystemClasses)
    };
	return CharacterArchiveStruct;
}

void FCharacterArchiveStruct::LoadArchiveStruct(AARPGCharacter* Character,
	FCharacterArchiveStruct CharacterArchiveStruct)
{
	Character->GetCharacterStatusComponent()->SetLevel(CharacterArchiveStruct.Level);
	Character->GetCharacterStatusComponent()->SetCoins(CharacterArchiveStruct.Coins);
	Character->GetCharacterStatusComponent()->SetHealthSpecialty(CharacterArchiveStruct.HealthSpecialty);
	Character->GetCharacterStatusComponent()->SetStaminaSpecialty(CharacterArchiveStruct.StaminaSpecialty);
	Character->GetCharacterStatusComponent()->SetAttackSpecialty(CharacterArchiveStruct.AttackSpecialty);
	Character->GetCharacterStatusComponent()->SetDefenseSpecialty(CharacterArchiveStruct.DefenseSpecialty);
	Character->GetCharacterStatusComponent()->SetToughnessSpecialty(CharacterArchiveStruct.ToughnessSpecialty);
	Character->GetCharacterStatusComponent()->SetCurrentHP(CharacterArchiveStruct.CurrentHP);
	Character->GetCharacterStatusComponent()->SetCurrentSP(CharacterArchiveStruct.CurrentSP);
	Character->GetCharacterStatusComponent()->SetCharacterState(CharacterArchiveStruct.CharacterState);

	TArray<ATranscendentalLawsSystem*> TranscendentalLawsSystems;
	for (auto TranscendentalLawsSystemSoftClass : CharacterArchiveStruct.TranscendentalLawsSystemClasses)
	{
		ATranscendentalLawsSystem* TranscendentalLawsSystem = Cast<ATranscendentalLawsSystem>(
            Character->GetWorld()->SpawnActor(TranscendentalLawsSystemSoftClass.LoadSynchronous()));
		TranscendentalLawsSystem->Init(Character, Character->GetCharacterCombatComponent());
		TranscendentalLawsSystems.Emplace(TranscendentalLawsSystem);
	}
	Character->GetCharacterCombatComponent()->SetTranscendentalLawsSystems(std::move(TranscendentalLawsSystems));
}
