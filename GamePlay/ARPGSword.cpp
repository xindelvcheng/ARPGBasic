// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGSword.h"

#include "ARPGCharacter.h"
#include "ARPGCharacterCombatComponent.h"
#include "ARPGDamageBoxComponent.h"
#include "ARPGStaticFunctions.h"
#include "cmath"
#include "Engine/EngineTypes.h"
#include "particles/ParticleSystemComponent.h"

// Sets default values
AARPGSword::AARPGSword()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	DamageBox = CreateDefaultSubobject<UARPGDamageBoxComponent>("DamageBox");
	DamageBox->SetupAttachment(RootComponent);

	TrailFXParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>("TrailFXParticleSystemComponent");
	TrailFXParticleSystemComponent->SetupAttachment(GetMesh());
}

USkeletalMeshComponent* AARPGSword::GetOwnerCharacterSkeletalMesh() const
{
	AARPGCharacter* OwnerCharacter = GetOwner<AARPGCharacter>();
	return OwnerCharacter ? OwnerCharacter->GetMesh() : nullptr;
}

// Called when the game starts or when spawned
void AARPGSword::BeginPlay()
{
	Super::BeginPlay();

	/*TODO*/
	TrailFXParticleSystemComponent->SetTemplate(TrailFXResource);
	AttachToComponent(GetOwnerCharacterSkeletalMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	// AddActorWorldOffset(FVector{500, 0, 0});

	if (AARPGCharacter* OwnerCharacter = GetOwner<AARPGCharacter>())
	{
		if (UARPGCharacterCombatComponent* CharacterCombatComponent = OwnerCharacter->GetCharacterCombatComponent())
		{
			CharacterCombatComponent->OnAttackStart().AddLambda([&]()
			{
				SwordState = ESwordStateEnum::Combat;
				Timer = 0;
				DamageBox->Activate(false);
				CircleDirection = CircleDirection == ECircleDirectionEnum::Clockwise
					                  ? ECircleDirectionEnum::Anticlockwise
					                  : ECircleDirectionEnum::Clockwise;
				TrailFXParticleSystemComponent->BeginTrails(
					TEXT("FX_Sword_Bottom"),TEXT("FX_Sword_Top"), ETrailWidthMode_FromCentre, 5);
			});
			CharacterCombatComponent->OnAttackEnd().AddLambda([&]()
			{
				SwordState = ESwordStateEnum::Idle;
				DamageBox->Deactivate();
				TrailFXParticleSystemComponent->EndTrails();
			});
		}
	}
}

void AARPGSword::Attack()
{
	FVector DummyTargetLocation = UARPGStaticFunctions::GetActorLocalTransform(
		GetOwnerCharacter(), FVector{500, 0, 0}, FRotator{},).GetLocation();
}

void AARPGSword::Defend()
{
}

// Called every frame
void AARPGSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timer += DeltaTime;

	if (AARPGCharacter* OwnerCharacter = GetOwnerCharacter())
	{
		switch (SwordState)
		{
		case ESwordStateEnum::Idle:
			SetActorLocation(
				OwnerCharacter->GetMesh()->GetSocketLocation("back_fur_2_rSocket") + OwnerCharacter->
				GetActorForwardVector() * -100);
			SetActorRelativeRotation(FRotator{0, 0, 0});
			break;
		case ESwordStateEnum::Combat:
			FVector DummyTargetLocation = UARPGStaticFunctions::GetActorLocalTransform(
				GetOwnerCharacter(), FVector{500, 0, 0}, FRotator{},).GetLocation();

			float const R = (DummyTargetLocation - GetOwnerCharacter()->GetActorLocation()).Size();
			float const Theta = PI / 2;
			/*注意这里需要将φ从左手坐标系转为右手坐标系*/
			float const Phi = CircleDirection == ECircleDirectionEnum::Clockwise
				                  ? PI - (6 * PI / 5 - Timer * 10)
				                  : PI - (-PI / 5 + Timer * 10);
			/*从球坐标系转为直角坐标系*/
			FVector SphereDirectionalPoint = FVector{
				R * sin(Theta) * cos(Phi), R * sin(Theta) * sin(Phi), R * cos(Theta)
			};
			SetActorRelativeLocation(SphereDirectionalPoint + FVector{0, 0, 100});
			SetActorRelativeRotation(SphereDirectionalPoint.Rotation() + FRotator{-90, 0, 0});
			break;
		}
	}
}

// Called to bind functionality to input
void AARPGSword::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
