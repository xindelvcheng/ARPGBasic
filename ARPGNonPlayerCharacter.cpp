// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGNonPlayerCharacter.h"


#include "ARPGBasicSettings.h"
#include "ARPGCoreSubsystem.h"
#include "Tasks/AITask_MoveTo.h"
#include "AIModule/Classes/AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIModule/Classes/BrainComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

// Sets default values
AARPGNonPlayerCharacter::AARPGNonPlayerCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    HPBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("HPBarWidget");
    HPBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

    if (UARPGBasicSettings::Get())
    {
        HPBarWidgetComponent->SetWidgetClass(UARPGBasicSettings::Get()->EnemyHPBarWidgetClass.LoadSynchronous());
        HPBarWidgetComponent->SetupAttachment(RootComponent);
        HPBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
        HPBarWidgetComponent->SetRelativeLocation(HPBarOffsetVector);
    }

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    UAISenseConfig_Sight* AISenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfig"));
    AISenseConfig->PeripheralVisionAngleDegrees = 120;
    FAISenseAffiliationFilter AISenseAffiliationFilter{true, true, false};
    AISenseConfig->DetectionByAffiliation = std::move(AISenseAffiliationFilter);
    AIPerceptionComponent->ConfigureSense(*AISenseConfig);
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this, &AARPGNonPlayerCharacter::BindToSensePerceptionUpdated);
}

void AARPGNonPlayerCharacter::ShowHPBar()
{
    if (HPBarWidgetComponent && HPBarWidgetComponent->GetUserWidgetObject())
    {
        HPBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
    }
}

void AARPGNonPlayerCharacter::HideHPBar()
{
    if (HPBarWidgetComponent && HPBarWidgetComponent->GetUserWidgetObject())
    {
        HPBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
    }
}

void AARPGNonPlayerCharacter::OnNPCDeath()
{
    AARPGMainCharacter* MainCharacter = UARPGCoreSubsystem::GetMainCharacter(GetWorld());
    if (MainCharacter)
    {
        MainCharacter->UpdateCoins(GetCoins());
        UpdateCoins(0);
    }


    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->GetBrainComponent()->StopLogic(FString());
    }
    if (GetController())
    {
        GetController()->UnPossess();
    }
}


// Called when the game starts or when spawned
void AARPGNonPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    HideHPBar();
    if (HPBarWidgetComponent && HPBarWidgetComponent->GetUserWidgetObject())
    {
        UARPGEnemyHPBarWidget* HPBarWidget = Cast<UARPGEnemyHPBarWidget>(HPBarWidgetComponent->GetUserWidgetObject());
        if (HPBarWidget)
        {
            HPBarWidget->BindToCharacter(this);
        }
    }

    bUseControllerRotationYaw = true;
    CharacterStatusComponent->OnCharacterDeath.AddDynamic(this, &AARPGNonPlayerCharacter::OnNPCDeath);
}

// Called every frame
void AARPGNonPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AARPGNonPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                          AController* EventInstigator, AActor* DamageCauser)
{
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (Cast<AARPGPlayerController>(EventInstigator))
    {
        ShowHPBar();

        if (auto AIController = Cast<AAIController>(GetController()))
        {
            if (AIController->GetBlackboardComponent())
            {
                AIController->GetBlackboardComponent()->SetValueAsObject("Target",EventInstigator->GetCharacter());
            }
        }
    }   
    return DamageAmount;
}


void AARPGNonPlayerCharacter::BindToSensePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    GetWorldTimerManager().ClearTimer(ForgetTargetTimerHandle);
    if (Stimulus.WasSuccessfullySensed())
    {
        if (auto MainCharacter = Cast<AARPGMainCharacter>(Actor))
        {
            if (FVector::Distance(GetActorLocation(), Actor->GetActorLocation()) > 1500 && Actor->GetVelocity().Size() <
                500)
            {
                FaceRotation(UKismetMathLibrary::Conv_VectorToRotator(
                    UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), Actor->GetActorLocation())));
            }
            else
            {
                LastHitBy = MainCharacter->GetController();
                if (auto AIController = Cast<AAIController>(GetController()))
                {
                    if (AIController->GetBlackboardComponent())
                    {
                        AIController->GetBlackboardComponent()->SetValueAsObject("Target",MainCharacter);
                    }
                }
            }
        }
    }
    else
    {
        GetWorldTimerManager().SetTimer(ForgetTargetTimerHandle, FTimerDelegate::CreateLambda([&]()
        {
            LastHitBy = nullptr;
        }), 5, false);
    }
}
