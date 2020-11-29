// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "particles/ParticleSystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGSpecialEffectsSubsystem.generated.h"

class AARPGCharacter;

UENUM(BlueprintType)
enum class EEffectCategory:uint8
{
    PositiveSoundEffects,
    NegativeSoundEffects,
    NeutralSoundEffects,
    PositiveVisualEffects,
    NegativeVisualEffects
};

/**
 * 
 */
UCLASS()
class UARPGSpecialEffectsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    template <typename T>
    void LoadEffectsAssets(TArray<T*>& Effects, TArray<TSoftObjectPtr<T>> EffectSoftObjectPtrs);

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> PositiveSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> NegativeSoundEffects;
    
    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<USoundCue*> NeutralSoundEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<UParticleSystem*> PositiveVisualEffects;

    UPROPERTY(BlueprintReadOnly,Category="ARPGSpecialEffectsSubsystem")
    TArray<UParticleSystem*> NegativeVisualEffects;

    static UARPGSpecialEffectsSubsystem* Get(UWorld* World)
    {
        if (World && World->GetGameInstance() && World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>())
        {
            return World->GetGameInstance()->GetSubsystem<UARPGSpecialEffectsSubsystem>();
        }

        return nullptr;
    }

    UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
    static void PlaySoundEffect2D(const UObject* WorldContextObject,EEffectCategory EffectCategory,int Index);

    UFUNCTION(BlueprintCallable,meta=(WorldContext=WorldContextObject))
    static void PlaySpecialEffectAtLocation(const UObject* WorldContextObject, EEffectCategory EffectCategory, int Index, FVector Location);
};

UCLASS(Blueprintable)
class AARPGSpecialEffectCreature : public AActor
{
    GENERATED_BODY()

protected:
    AARPGCharacter* OwnerCharacter;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    float DamageWeight = 0;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    float DamageBias = 10;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    UParticleSystem* DestroyVisualEffect;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect")
    USoundBase* DestroySoundEffect;


public:
    UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffect")
    void SetOwnerCharacter(AARPGCharacter* NewOwnerCharacter) { OwnerCharacter = NewOwnerCharacter; }

    UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffect")
    AARPGCharacter* GetOwnerCharacter() const { return OwnerCharacter; }

   

    UFUNCTION(BlueprintCallable,Category="ARPGSpecialEffectCreature")
    static AARPGSpecialEffectCreature* SpawnARPGSpecialEffectCreature(TSubclassOf<AARPGSpecialEffectCreature> CreatureClass,
                                               FTransform Transform, AARPGCharacter* OwnerCharacter);
};

UCLASS(Blueprintable)
class AARPGSimpleMovableCauseDamageSpecialEffectCreature : public AARPGSpecialEffectCreature
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
    float MoveRate = 10;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGSpecialEffect",meta=(AllowPrivateAccess))
    UBoxComponent* DamageDetectionCollisionBox;
public:
    AARPGSimpleMovableCauseDamageSpecialEffectCreature();

    UFUNCTION()
    void BindToActorBeginOverlap(AActor* OtherActor) ;

    virtual void BeginPlay() override;
};
