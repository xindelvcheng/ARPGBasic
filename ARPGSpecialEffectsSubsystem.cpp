// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSpecialEffectsSubsystem.h"

#include "ARPGBasicSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"

template <typename T>
void UARPGSpecialEffectsSubsystem::LoadEffectsAssets(TArray<T*>& Effects,
                                                     TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs)
{
    for (TSoftObjectPtr<T> Effect : EffectSoftObjectPtrs)
    {
        Effects.Emplace(Effect.LoadSynchronous());
    }
}

void UARPGSpecialEffectsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UARPGBasicSettings::Get())
    {
        const auto ARPGBasicSettings = UARPGBasicSettings::Get();
        LoadEffectsAssets(PositiveSoundEffects, ARPGBasicSettings->PositiveSoundEffects);
        LoadEffectsAssets(NegativeSoundEffects, ARPGBasicSettings->NegativeSoundEffects);
        LoadEffectsAssets(NeutralSoundEffects, ARPGBasicSettings->NeutralSoundEffects);
        LoadEffectsAssets(PositiveVisualEffects, ARPGBasicSettings->PositiveVisualEffects);
        LoadEffectsAssets(NegativeVisualEffects, ARPGBasicSettings->NegativeVisualEffects);
    }
}

void UARPGSpecialEffectsSubsystem::PlaySoundEffect2D(const UObject* WorldContextObject, EEffectCategory EffectCategory,
                                                     int Index)
{
    if (UARPGSpecialEffectsSubsystem* ARPGSpecialEffectsSubsystemInstance = Get(WorldContextObject->GetWorld()))
    {
        switch (EffectCategory)
        {
        case EEffectCategory::PositiveSoundEffects:
            UGameplayStatics::PlaySound2D(WorldContextObject,
                                          ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects.IsValidIndex(Index)
                                              ? ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects[Index]
                                              : nullptr);
            break;
        case EEffectCategory::NegativeSoundEffects:
            UGameplayStatics::PlaySound2D(WorldContextObject,
                                          ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects.IsValidIndex(Index)
                                              ? ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects[Index]
                                              : nullptr);
            break;
        case EEffectCategory::NeutralSoundEffects:
            UGameplayStatics::PlaySound2D(WorldContextObject,
                                          ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects.IsValidIndex(Index)
                                              ? ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects[Index]
                                              : nullptr);
            break;
        default:
            UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，PlaySoundEffect2D函数只能播放音效"));
        }
    }
}


void UARPGSpecialEffectsSubsystem::PlaySpecialEffectAtLocation(const UObject* WorldContextObject,
                                                               EEffectCategory EffectCategory, int Index,
                                                               FVector Location)
{
    if (UARPGSpecialEffectsSubsystem* ARPGSpecialEffectsSubsystemInstance = Get(WorldContextObject->GetWorld()))
    {
        switch (EffectCategory)
        {
        case EEffectCategory::PositiveSoundEffects:
            UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
                                                  ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects.
                                                  IsValidIndex(Index)
                                                      ? ARPGSpecialEffectsSubsystemInstance->PositiveSoundEffects[Index]
                                                      : nullptr, Location);
            break;
        case EEffectCategory::NegativeSoundEffects:
            UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
                                                  ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects.
                                                  IsValidIndex(Index)
                                                      ? ARPGSpecialEffectsSubsystemInstance->NegativeSoundEffects[Index]
                                                      : nullptr, Location);
            break;
        case EEffectCategory::NeutralSoundEffects:
            UGameplayStatics::PlaySoundAtLocation(WorldContextObject,
                                                  ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects.IsValidIndex(
                                                      Index)
                                                      ? ARPGSpecialEffectsSubsystemInstance->NeutralSoundEffects[Index]
                                                      : nullptr, Location);
            break;
        case EEffectCategory::PositiveVisualEffects:
            UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject,
                                                     ARPGSpecialEffectsSubsystemInstance->PositiveVisualEffects.
                                                     IsValidIndex(Index)
                                                         ? ARPGSpecialEffectsSubsystemInstance->PositiveVisualEffects[
                                                             Index]
                                                         : nullptr, Location);
            break;
        case EEffectCategory::NegativeVisualEffects:
            UGameplayStatics::SpawnEmitterAtLocation(WorldContextObject,
                                                     ARPGSpecialEffectsSubsystemInstance->NegativeVisualEffects.
                                                     IsValidIndex(Index)
                                                         ? ARPGSpecialEffectsSubsystemInstance->NegativeVisualEffects[
                                                             Index]
                                                         : nullptr, Location);
            break;
        }
    }
}

void AARPGSpecialEffectCreature::SpawnARPGSpecialEffectCreature(UWorld* World,
                                                                TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
                                                                FTransform const& Transform,
                                                                AARPGCharacter* OwnerCharacter,
                                                                const FActorSpawnParameters& SpawnParameters)
{
    auto SpecialEffectCreature = Cast<AARPGSpecialEffectCreature>(
        World->SpawnActor(CreatureClass, &Transform, SpawnParameters));
    SpecialEffectCreature->SetOwnerCharacter(OwnerCharacter);
}

void AARPGSpecialEffectCreature::SpawnARPGSpecialEffectCreature(const UObject* WorldContextObject,
                                                                TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
                                                                FTransform const& Transform,
                                                                AARPGCharacter* OwnerCharacter)
{
    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.Owner = OwnerCharacter;
    SpawnARPGSpecialEffectCreature(WorldContextObject->GetWorld(), CreatureClass, Transform, OwnerCharacter,
                                   ActorSpawnParameters);
}

AARPGSimpleMovableCauseDamageSpecialEffectCreature::AARPGSimpleMovableCauseDamageSpecialEffectCreature()
{
    DamageDetectionCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageDetectionCollisionBox"));
    DamageDetectionCollisionBox->SetupAttachment(RootComponent);
    PrimaryActorTick.bCanEverTick = true;
}

void AARPGSimpleMovableCauseDamageSpecialEffectCreature::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (Cast<AARPGCharacter>(OtherActor) == OwnerCharacter
        || Cast<AGameItem>(OtherActor)
        || (OtherActor->GetClass() == this->GetClass() && OtherActor->GetOwner() == this->GetOwner()))
    {
        return;
    }

    GetWorld()->DestroyActor(this);
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyVisualEffect, GetActorLocation());
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DestroySoundEffect, GetActorLocation());

    verify(OwnerCharacter);
    UGameplayStatics::ApplyDamage(
        OtherActor, OwnerCharacter->GetCharacterStatusComponent()->GetAttack() * DamageWeight + DamageBias,
        OwnerCharacter->GetController(), this, DamageType); //DestroyActor销毁Actor将会在下一帧，所以此处还可以使用
}

void AARPGSimpleMovableCauseDamageSpecialEffectCreature::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    AddActorLocalOffset(FVector{1, 0, 0} * MoveSpeed);
}
