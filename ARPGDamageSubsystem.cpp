// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageSubsystem.h"

#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"

UDamageDetectRecord* UDamageDetectRecord::Create(TWeakObjectPtr<USceneComponent> DamageCenterSceneComponent,
                                                 AARPGCharacter* InstigatorCharacter,
                                                 FDamageDetectDescriptionStruct DamageDetectDescription)
{
	UDamageDetectRecord* Record = NewObject<UDamageDetectRecord>();
	Record->DamageCenter = DamageCenterSceneComponent;
	Record->Instigator = InstigatorCharacter;
	Record->InstigatorPlayerController = Cast<APlayerController>(InstigatorCharacter->GetController());
	Record->InstigatorStatusComponent = Cast<UCharacterStatusComponent>(
		InstigatorCharacter->GetCharacterStatusComponent());
	Record->BaseAttack = Record->InstigatorStatusComponent->GetAttack();

	Record->DamageDetectDescriptionStruct = DamageDetectDescription;

	Record->LastFrameDamageCenterTransform = DamageCenterSceneComponent->GetComponentTransform();
	Record->CurrentFrameDamageCenterTransform = DamageCenterSceneComponent->GetComponentTransform();

#ifdef WITH_EDITOR
	Record->DrawDebugType = Record->bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
#endif

	return Record;
}

void UDamageDetectRecord::Tick(float)
{
	if (!Instigator.IsValid() || !DamageCenter.IsValid() || !InstigatorStatusComponent.IsValid() || !
		InstigatorPlayerController.IsValid())
	{
		UE_LOG(LogTemp,Warning,TEXT("有一条DamageDetectRecord因弱指针无效而被跳过"));
		return;
	}
	CurrentFrameDamageCenterTransform = DamageCenter->GetComponentTransform();
	UKismetSystemLibrary::BoxTraceMultiForObjects(Instigator->GetWorld(), LastFrameDamageCenterTransform.GetLocation(),
	                                              CurrentFrameDamageCenterTransform.GetLocation(),
	                                              DamageDetectDescriptionStruct.DamageBoxHalfSizeInTrace,
	                                              CurrentFrameDamageCenterTransform.Rotator(), DetectObjectTypes,
	                                              false, DamageDetectDescriptionStruct.ActorsToIgnore,
	                                              DrawDebugType,
	                                              HitResults,
	                                              true);
	for (int i = 0; i < HitResults.Num(); i++)
	{
		AActor* HitActor = HitResults[i].GetActor();
		if (!HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			OnDamageDetected(HitResults[i]);
		}
	}

	LastFrameDamageCenterTransform = DamageCenter->GetComponentTransform();
}

void UDamageDetectRecord::OnDamageDetected(FHitResult HitResult)
{
	DamageDetectedDelegate.ExecuteIfBound(this,HitResult);
	InstigatorStatusComponent->OnAttackHitActor.Broadcast(HitResult);
	
	const float BaseDamage = DamageDetectDescriptionStruct.DamageWeight * BaseAttack + DamageDetectDescriptionStruct.
            DamageBias + Instigator->GetVelocity().Size() *
            DamageDetectDescriptionStruct.VelocityDamageBonusWeight;
	if (CauseDamage)
	{
		AActor* HitActor = HitResult.GetActor();
		UGameplayStatics::ApplyPointDamage(
            HitActor, BaseDamage,
            HitResult.Location, HitResult,
            InstigatorPlayerController.Get(),
            Instigator.Get(), DamageDetectDescriptionStruct.DamageTypeClass
                                  ? DamageDetectDescriptionStruct.DamageTypeClass
                                  : InstigatorStatusComponent->GetMeleeDamageType());
	}
}

bool UARPGDamageSubsystem::Tick(float DeltaSeconds)
{
	for (int i = Records.Num() - 1; i >= 0; --i)
	{
		Records[i]->Tick(DeltaSeconds);
	}

	return true;
}

UARPGDamageSubsystem* UARPGDamageSubsystem::Get(UWorld* World)
{
	if (World && World->GetGameInstance())
	{
		return World->GetGameInstance()->GetSubsystem<UARPGDamageSubsystem>();
	}

	return nullptr;
}

void UARPGDamageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UARPGDamageSubsystem::Tick));
}

UDamageDetectRecord* UARPGDamageSubsystem::RegisterToDamageDetect(
	TWeakObjectPtr<USceneComponent> DamageCenterSceneComponent, AARPGCharacter* InstigatorCharacter,
	FDamageDetectDescriptionStruct DamageDetectDescription)
{
	UDamageDetectRecord* Record = UDamageDetectRecord::Create(DamageCenterSceneComponent, InstigatorCharacter,
	                                                          DamageDetectDescription);
	Record->DamageDetectedDelegate.BindLambda([&](UDamageDetectRecord* DamageDetectRecord,FHitResult HitResult)
	{
		DamageDetectedEvent.Broadcast(DamageDetectRecord,HitResult);
	});
	Records.Emplace(Record);
	return Record;
}

UDamageDetectRecord* UARPGDamageSubsystem::RegisterToDamageDetect(TWeakObjectPtr<AActor> DamageCenter,
                                                                  AARPGCharacter* InstigatorCharacter,
                                                                  FDamageDetectDescriptionStruct
                                                                  DamageDetectDescription)
{
	if (!DamageCenter->GetRootComponent())
	{
		UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，DamageDetect的没有RootComponent"), 15, FColor::Red);
	}
	return RegisterToDamageDetect(DamageCenter->GetRootComponent(), InstigatorCharacter, DamageDetectDescription);
}

void UARPGDamageSubsystem::UnRegisterToDamageDetect(UDamageDetectRecord* Record)
{
	Record->DamageDetectedDelegate.Unbind();
	Records.Remove(Record);
}
