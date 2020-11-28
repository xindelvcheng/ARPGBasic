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


void AARPGSpecialEffectCreature::SpawnARPGSpecialEffectCreature(TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
                                                                FTransform Transform,
                                                                AARPGCharacter* OwnerCharacter)
{
    FActorSpawnParameters ActorSpawnParameters;
    ActorSpawnParameters.Instigator = OwnerCharacter;
    ActorSpawnParameters.Owner = OwnerCharacter;
    ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    auto SpecialEffectCreature = OwnerCharacter->GetWorld()->SpawnActor<AARPGSpecialEffectCreature>(CreatureClass, Transform, ActorSpawnParameters);
    if (SpecialEffectCreature)
    {
        SpecialEffectCreature->SetOwnerCharacter(OwnerCharacter);
    }
}

AARPGSimpleMovableCauseDamageSpecialEffectCreature::AARPGSimpleMovableCauseDamageSpecialEffectCreature()
{
    DamageDetectionCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageDetectionCollisionBox"));
    RootComponent = DamageDetectionCollisionBox;
    PrimaryActorTick.bCanEverTick = true;
}

void AARPGSimpleMovableCauseDamageSpecialEffectCreature::BeginPlay()
{
    Super::BeginPlay();
    FCollisionDelegate CollisionDelegate;
    CollisionDelegate.BindDynamic(this,&AARPGSimpleMovableCauseDamageSpecialEffectCreature::BindToActorBeginOverlap);
    if (GetInstigator())
    {
        UARPGGameInstanceSubsystem::MoveActorTowardsDirectionFinishOnCollision(this,GetInstigator()->GetActorForwardVector(),{},CollisionDelegate,MoveRate);
    }else
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(FString::Printf(TEXT("实例化类%s时未使用SpawnCreature，且没有指定拥有者，将不会移动。"),*GetFullName()));
    }
    SetLifeSpan(1);
}

void AARPGSimpleMovableCauseDamageSpecialEffectCreature::BindToActorBeginOverlap(AActor* OtherActor)
{
    if (OwnerCharacter)
    {
        UGameplayStatics::ApplyDamage(
        OtherActor, OwnerCharacter->GetCharacterStatusComponent()->GetAttack() * DamageWeight + DamageBias,
        OwnerCharacter->GetController(), this, DamageType);
    }else
    {
        UGameplayStatics::ApplyDamage(
        OtherActor, 1,
        nullptr, this, DamageType);
    }
    
    const FVector ActorLocation = GetActorLocation();
    GetWorld()->DestroyActor(this);
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyVisualEffect, ActorLocation);
    UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DestroySoundEffect, ActorLocation);
}
