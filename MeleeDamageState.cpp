// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeDamageState.h"


#include "ARPGCharacter.h"
#include "CharacterStatusComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMeleeDamageState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    Player = Cast<AARPGCharacter>(MeshComp->GetOwner());
    if (Player)
    {
        ActorsToIgnore = {MeshComp->GetOwner()};
        EventInstigator = Player->GetController();
        if (Weapon)
        {
            LastLocation1 = Weapon->GetSocketLocation(DamageCenterSocketName);
        }
        else
        {
            LastLocation1 = Player->GetMesh()->GetSocketLocation(DamageCenterSocketName);
        }
        StatusComponent = Cast<UCharacterStatusComponent>(
            Player->FindComponentByClass(UCharacterStatusComponent::StaticClass()));
    }

    HitActors.Empty();
}

void UMeleeDamageState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
    if (Player)
    {
        if (Weapon)
        {
            CurrentLocation1 = Weapon->GetSocketLocation(DamageCenterSocketName);
            CurrentRotation1 = Weapon->GetSocketRotation(DamageCenterSocketName);
        }
        else
        {
            CurrentLocation1 = Player->GetMesh()->GetSocketLocation(DamageCenterSocketName);
            CurrentRotation1 = Player->GetMesh()->GetSocketRotation(DamageCenterSocketName);
        }


        UKismetSystemLibrary::BoxTraceMultiForObjects(Player->GetWorld(), LastLocation1,
                                                      CurrentLocation1, DamageBoxHalfSizeInTrace,
                                                      CurrentRotation1, ObjectTypes,
                                                      false, ActorsToIgnore,
                                                      bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
                                                      HitResult,
                                                      true);

        if (StatusComponent)
        {
            BaseAttack = StatusComponent->GetAttack();
        }

        const float BaseDamage = DamageWeight * BaseAttack + DamageBias + Player->GetVelocity().Size() *
            VelocityDamageBonusWeight;

        for (int i = 0; i < HitResult.Num(); i++)
        {
            AActor* HitActor = HitResult[i].GetActor();
            if (!HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);
                if (CauseDamage)
                {
                    UGameplayStatics::ApplyPointDamage(
                        HitActor, BaseDamage,
                        HitResult[i].Location, HitResult[i],
                        EventInstigator,
                        Player, DamageTypeClass
                                    ? DamageTypeClass
                                    : StatusComponent->GetMeleeDamageType());
                }
                if (StatusComponent)
                {
                    StatusComponent->OnAttackHitActor.Broadcast(HitResult[i]);
                }
            }
        }
        if (Weapon)
        {
            LastLocation1 = Weapon->GetSocketLocation(DamageCenterSocketName);
        }
        else
        {
            LastLocation1 = Player->GetMesh()->GetSocketLocation(DamageCenterSocketName);
        }
    }
}

void UMeleeDamageState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    Super::NotifyEnd(MeshComp, Animation);
}
