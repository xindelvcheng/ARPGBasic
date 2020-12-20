// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGDamageSubsystem.h"

#include "ARPGCharacter.h"
#include "ARPGGameInstanceSubsystem.h"
#include "ARPGDamageBoxComponent.h"

UDamageDetectRecord* UDamageDetectRecord::Create(UARPGDamageBoxComponent* DamageCenterSceneComponent,
                                                 AARPGCharacter* InstigatorCharacter,
                                                 FDamageDetectedDelegate DelegateOnDamageDetected,
                                                 FDamageDetectDescriptionStruct DamageDetectDescription)
{
	UDamageDetectRecord* Record = NewObject<UDamageDetectRecord>();
	Record->DamageCenterComponent = DamageCenterSceneComponent;
	Record->Instigator = InstigatorCharacter;
	Record->InstigatorPlayerController = Cast<APlayerController>(InstigatorCharacter->GetController());
	Record->InstigatorStatusComponent = Cast<UCharacterStatusComponent>(
		InstigatorCharacter->GetCharacterStatusComponent());
	Record->BaseAttack = Record->InstigatorStatusComponent->GetAttack();
	Record->DamageDetectDescriptionStruct = DamageDetectDescription;
	Record->LastFrameDamageCenterTransform = DamageCenterSceneComponent->GetComponentTransform();
	Record->CurrentFrameDamageCenterTransform = DamageCenterSceneComponent->GetComponentTransform();
	Record->ActorsToIgnore.AddUnique(InstigatorCharacter);
	Record->DamageDetectedEvent.Add(DelegateOnDamageDetected);

	return Record;
}

void UDamageDetectRecord::Tick(float)
{
	if (!Instigator.IsValid() || !DamageCenterComponent.IsValid() || !InstigatorStatusComponent.IsValid() || !
		InstigatorPlayerController.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("有一条DamageDetectRecord因弱指针无效而被跳过"));
		return;
	}

	CurrentFrameDamageCenterTransform = DamageCenterComponent->GetComponentTransform();

	if (DamageDetectDescriptionStruct.bUseDamageCenterComponentCurrentBoundsAsDetectBound)
	{
		const FBoxSphereBounds Bounds = DamageCenterComponent->CalcLocalBounds();
		DamageDetectDescriptionStruct.DamageBoxHalfSizeInTrace = Bounds.BoxExtent * DamageCenterComponent->
			GetComponentScale();
		CurrentFrameDamageCenterTransform.SetLocation(Bounds.Origin + DamageCenterComponent->GetComponentLocation());
	}


	UKismetSystemLibrary::BoxTraceMultiForObjects(Instigator->GetWorld(), LastFrameDamageCenterTransform.GetLocation(),
	                                              CurrentFrameDamageCenterTransform.GetLocation(),
	                                              DamageDetectDescriptionStruct.DamageBoxHalfSizeInTrace,
	                                              CurrentFrameDamageCenterTransform.Rotator(),
	                                              DetectObjectTypes,
	                                              false,
	                                              ActorsToIgnore,
	                                              bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
	                                              HitResults,
	                                              true);
	for (int i = 0; i < HitResults.Num(); i++)
	{
		AActor* HitActor = HitResults[i].GetActor();
		if (HitResults[i].GetComponent() != DamageCenterComponent && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			OnDamageDetected(HitResults[i]);
		}
	}

	LastFrameDamageCenterTransform = CurrentFrameDamageCenterTransform;
}

void UDamageDetectRecord::OnDamageDetected(FHitResult HitResult)
{
	DamageDetectedEvent.Broadcast(this, HitResult);
	InstigatorStatusComponent->OnAttackHitActor.Broadcast(HitResult);

	const float BaseDamage = DamageDetectDescriptionStruct.DamageWeight * BaseAttack + DamageDetectDescriptionStruct.
		DamageBias + Instigator->GetVelocity().Size() *
		DamageDetectDescriptionStruct.VelocityDamageBonusWeight;

	if (UARPGDamageBoxComponent* DamageBoxComponent = Cast<UARPGDamageBoxComponent>(HitResult.GetComponent()))
	{
		DamageCenterComponent->ElementInteract(DamageBoxComponent);
	}

	if (DamageDetectDescriptionStruct.CauseDamage)
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
	UARPGDamageBoxComponent* DamageCenterSceneComponent, AARPGCharacter* InstigatorCharacter,
	FDamageDetectedDelegate DelegateOnDamageDetected,
	FDamageDetectDescriptionStruct DamageDetectDescription)
{
	UDamageDetectRecord* Record = UDamageDetectRecord::Create(DamageCenterSceneComponent, InstigatorCharacter,
	                                                          DelegateOnDamageDetected,
	                                                          DamageDetectDescription);
	Records.Emplace(Record);
	return Record;
}

UDamageDetectRecord* UARPGDamageSubsystem::RegisterToDamageDetect(AActor* DamageCenterActor,
                                                                  AARPGCharacter* InstigatorCharacter,
                                                                  FDamageDetectedDelegate DelegateOnDamageDetected,
                                                                  FDamageDetectDescriptionStruct
                                                                  DamageDetectDescription)
{
	if (UARPGDamageBoxComponent* DamageBoxComponent = Cast<UARPGDamageBoxComponent>(
		DamageCenterActor->GetComponentByClass(UARPGDamageBoxComponent::StaticClass())))
	{
		return RegisterToDamageDetect(DamageBoxComponent,
		                              InstigatorCharacter, DelegateOnDamageDetected,
		                              DamageDetectDescription);
	}
	UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误，DamageDetect的没有UARPGDamageBoxComponent"), 15, FColor::Red);
	return nullptr;
}

void UARPGDamageSubsystem::UnRegisterToDamageDetect(UDamageDetectRecord* Record)
{
	Records.Remove(Record);
}
