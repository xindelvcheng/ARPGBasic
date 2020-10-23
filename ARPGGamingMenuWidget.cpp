// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPGGamingMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "ARPGBasicSettings.h"
#include "ARPGMainCharacter.h"
#include "ARPGGameInstanceSubsystem.h"
#include "CharacterStatusComponent.h"
#include "TranscendentalCombatComponent.h"
#include "TranscendentalLawsSystem.h"


bool UARPGGamingMenuWidget::Initialize()
{
    Super::Initialize();
    if (!HorizontalBox_TitleOptions)
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen("ARPGGamingMenuWidget蓝图类与C++类不对应");
        return false;
    }

    return true;
}

void UARPGGamingMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    for (UWidget* Widget : HorizontalBox_TitleOptions->GetAllChildren())
    {
        auto GamingMenuTitleItemWidget = Cast<UARPGGamingMenuPageWidget>(Widget);
        GamingMenuTitleItemWidget->ScrollBox_Content = ScrollBox_Content;
        GamingMenuTitleItemWidget->ParentMenuWidget = this;
        GamingMenuTitleItemWidget->OnClickTitleOption.BindLambda([&](UARPGGamingMenuPageWidget* Instigator)
        {
            if (Instigator == CurrentActiveMenuPage)
            {
                return;
            }

            ScrollBox_Content->ClearChildren();
            Instigator->NativeBeSelected();

            if (CurrentActiveMenuPage)
            {
                CurrentActiveMenuPage->NativeNotSelected();
            }
            CurrentActiveMenuPage = Instigator;
        });
    }
    if (!CurrentActiveMenuPage && HorizontalBox_TitleOptions->GetChildrenCount() > 0)
    {
        auto GamingMenuPageWidget = Cast<UARPGGamingMenuPageWidget>(HorizontalBox_TitleOptions->GetChildAt(0));
        if (GamingMenuPageWidget)
        {
            GamingMenuPageWidget->NativeBeSelected();
            CurrentActiveMenuPage = GamingMenuPageWidget;
        }
    }
}

bool UARPGGamingMenuContentItemWidget::Initialize()
{
    Super::Initialize();

    Button_Promote->OnClicked.AddDynamic(this, &UARPGGamingMenuContentItemWidget::OnClickButton_Promote);
    return true;
}

void UARPGGamingMenuContentItemWidget::OnClickButton_Promote()
{
    OnPromote.ExecuteIfBound();
}


bool UARPGGamingMenuPageWidget::Initialize()
{
    Super::Initialize();

    if (!TextBlock_PageTitleName || GetPageTitleName().IsEmpty())
    {
        return false;
    }

    TextBlock_PageTitleName->SetText(GetPageTitleName());
    return true;
}

void UARPGGamingMenuPageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Button_Option->OnClicked.AddDynamic(this, &UARPGGamingMenuPageWidget::Button_OptionOnClick);
    SetRenderOpacity(0.7);

    if (!GamingMenuContentItemWidgetClass && UARPGBasicSettings::Get())
    {
        GamingMenuContentItemWidgetClass = UARPGBasicSettings::Get()->GamingMenuContentItemWidgetClass.
                                                                      LoadSynchronous();
    }
}

void UARPGGamingMenuPageWidget::NativeBeSelected()
{
    SetRenderOpacity(1);

    BeSelected();
}

void UARPGGamingMenuPageWidget::NativeNotSelected()
{
    SetRenderOpacity(0.7);
    NotSelected();
}

void UARPGGamingMenuPageWidget::Button_OptionOnClick()
{
    OnClickTitleOption.ExecuteIfBound(this);
}

void UCharacterPropertiesPage::NativeConstruct()
{
    Super::NativeConstruct();

    if (UARPGGameInstanceSubsystem::Get(GetWorld()))
    {
        if (UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld()))
        {
            MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
            MainCharacterStatusComponent = MainCharacter->GetCharacterStatusComponent();
        }
        else
        {
            UARPGGameInstanceSubsystem::Get(GetWorld())->OnPlayerSetupEnd.AddLambda([&]()
            {
                MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
                MainCharacterStatusComponent = MainCharacter->GetCharacterStatusComponent();
            });
        }
    }

    if (!CharacterPropertiesPageItemWidgetClass)
    {
        CharacterPropertiesPageItemWidgetClass = GamingMenuContentItemWidgetClass;
    }
}

void UCharacterPropertiesPage::NativeBeSelected()
{
    Super::NativeBeSelected();

    if (!MainCharacter || ! MainCharacterStatusComponent)
    {
        return;
    }

    TArray<UARPGGamingMenuContentItemWidget*> ContentItemWidgets;
    for (int i = 0; i < 5; ++i)
    {
        auto MenuContentItemWidget = Cast<UARPGGamingMenuContentItemWidget>(
            CreateWidget(this, CharacterPropertiesPageItemWidgetClass));
        if (MenuContentItemWidget)
        {
            ContentItemWidgets.Emplace(MenuContentItemWidget);
        }
    }

    ContentItemWidgets[0]->SetupMenuContentItemWidget(
        TEXT("生命"),
        FString::Printf(TEXT("%d/%d"), MainCharacter->GetCurrentHP(), MainCharacter->GetMaxHP()),
        FString::FromInt(MainCharacterStatusComponent->GetHealthSpecialty()),
        [&]()
        {
            MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Health);
        }
    );

    ContentItemWidgets[1]->SetupMenuContentItemWidget(
        TEXT("灵气"),
        FString::Printf(TEXT("%d/%d"), MainCharacter->GetCurrentSP(), MainCharacter->GetMaxSP()),
        FString::FromInt(MainCharacterStatusComponent->GetStaminaSpecialty()),
        [&]()
        {
            MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Stamina);
        }
    );

    ContentItemWidgets[2]->SetupMenuContentItemWidget(
        TEXT("灵气强度"),
        MainCharacterStatusComponent->GetAttack(),
        MainCharacterStatusComponent->GetAttackSpecialty(),
        [&]()
        {
            MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Attack);
        }
    );

    ContentItemWidgets[3]->SetupMenuContentItemWidget(
        TEXT("护体灵气"),
        MainCharacterStatusComponent->GetDefense(),
        MainCharacterStatusComponent->GetDefenseSpecialty(),
        [&]()
        {
            MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Defense);
        }
    );

    ContentItemWidgets[4]->SetupMenuContentItemWidget(
        TEXT("韧性"),
        MainCharacterStatusComponent->GetToughness(),
        MainCharacterStatusComponent->GetToughnessSpecialty(),
        [&]()
        {
            MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Toughness);
        }
    );


    for (int i = 0; i < 5; ++i)
    {
        ScrollBox_Content->InsertChildAt(i, ContentItemWidgets[i]);
    }
}


void UCharacterTranscendentalLawsSystemPage::NativeConstruct()
{
    Super::NativeConstruct();

    if (UARPGGameInstanceSubsystem::Get(GetWorld()))
    {
        if (UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld()))
        {
            MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
            MainCharacterCombatComponent = MainCharacter->GetCharacterCombatComponent();
        }
        else
        {
            UARPGGameInstanceSubsystem::Get(GetWorld())->OnPlayerSetupEnd.AddLambda([&]()
            {
                MainCharacter = UARPGGameInstanceSubsystem::GetMainCharacter(GetWorld());
                MainCharacterCombatComponent = MainCharacter->GetCharacterCombatComponent();
            });
        }
    }

    if (!CharacterTranscendentalLawsSystemPageItemWidgetClass)
    {
        CharacterTranscendentalLawsSystemPageItemWidgetClass = GamingMenuContentItemWidgetClass;
    }
}

void UCharacterTranscendentalLawsSystemPage::NativeBeSelected()
{
    Super::NativeBeSelected();

    if (!MainCharacter || ! MainCharacterCombatComponent)
    {
        return;
    }

    TArray<UARPGGamingMenuContentItemWidget*> ContentItemWidgets;
    for (ATranscendentalLawsSystem* TranscendentalLawsSystem : MainCharacterCombatComponent->
         GetAllTranscendentalLawsSystems())
    {
        auto MenuContentItemWidget = Cast<UARPGGamingMenuContentItemWidget>(
            CreateWidget(this, CharacterTranscendentalLawsSystemPageItemWidgetClass));
        if (MenuContentItemWidget)
        {
            MenuContentItemWidget->SetupMenuContentItemWidget(TranscendentalLawsSystem->GetDisplayName(),
                                                              FText::FromName(TranscendentalLawsSystem->GetCategory()),
                                                              TranscendentalLawsSystem->
                                                              GetTranscendentalLawsAttainmentText(), []()
                                                              {
                                                                  if (GEngine)
                                                                  {
                                                                      GEngine->AddOnScreenDebugMessage(
                                                                          -1, 5, FColor::Yellow,TEXT("提示仙道信息"));
                                                                  }
                                                              });
            ContentItemWidgets.Emplace(MenuContentItemWidget);
            ScrollBox_Content->AddChild(MenuContentItemWidget);
        }
    }
}

void UCharacterReturnToGamePage::NativeBeSelected()
{
    Super::NativeBeSelected();

    ParentMenuWidget->RemoveFromParent();
    if (AARPGPlayerController* MainCharacterController =
        UARPGGameInstanceSubsystem::GetMainCharacterController(GetWorld()))
    {
        MainCharacterController->SetInputMode(FInputModeGameOnly{});
        MainCharacterController->bShowMouseCursor = false;
    }
}

void UCharacterQuitGamePage::NativeBeSelected()
{
    Super::NativeBeSelected();

    UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
}
