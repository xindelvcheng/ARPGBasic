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
#include "Blueprint/WidgetTree.h"


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

    if (!Button_1)
    {
        UARPGGameInstanceSubsystem::PrintLogToScreen(TEXT("错误！ARPGGamingMenuContentItemWidget蓝图类与C++类不对应！"));
        return false;
    }

    Button_1->OnClicked.AddDynamic(this, &UARPGGamingMenuContentItemWidget::OnClickButton1);
    return true;
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

UARPGGamingMenuContentItemWidget* UARPGGamingMenuPageWidget::AddContentItem(
    TSubclassOf<UARPGGamingMenuContentItemWidget> PageItemWidgetClass,
    const FGamingMenuContentItemStruct& GamingMenuContentItemStruct)
{
    UARPGGamingMenuContentItemWidget* MenuContentItemWidget = Cast<UARPGGamingMenuContentItemWidget>(
        CreateWidget(this, PageItemWidgetClass));
    if (MenuContentItemWidget)
    {
        MenuContentItemWidget->TextBlock_LeftText->SetText(GamingMenuContentItemStruct.LeftText);
        MenuContentItemWidget->TextBlock_MiddleText->SetText(GamingMenuContentItemStruct.MiddleText);
        MenuContentItemWidget->TextBlock_Button1Text->SetText(GamingMenuContentItemStruct.ButtonText);
        MenuContentItemWidget->ClickButton1.BindLambda(GamingMenuContentItemStruct.ButtonClickEffect);
        ScrollBox_Content->AddChild(MenuContentItemWidget);
        return MenuContentItemWidget;
    }
    return nullptr;
}

void UARPGGamingMenuPageWidget::AddSeparator(TSubclassOf<UARPGGamingMenuContentItemWidget> ContentSeparatorWidgetClass,
                                             FText LeftText, FText MiddleText, FText RightText
)
{
    UARPGGamingMenuContentItemWidget* SeparatorWidget = Cast<UARPGGamingMenuContentItemWidget>(
        CreateWidget(this, ContentSeparatorWidgetClass));
    if (SeparatorWidget)
    {
        SeparatorWidget->TextBlock_LeftText->SetText(LeftText);
        SeparatorWidget->TextBlock_MiddleText->SetText(MiddleText);
        SeparatorWidget->TextBlock_Button1Text->SetText(RightText);
        SeparatorWidget->Button_1->SetVisibility(ESlateVisibility::Hidden);
        ScrollBox_Content->AddChild(SeparatorWidget);
    }
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


    AddSeparator(CharacterPropertiesPageItemWidgetClass, FText::FromString(TEXT("境界")),
                 FText::FromString(MainCharacter->GetLevelName()), FText());

    AddContentItem(TEXT("生命"),
                   FString::Printf(TEXT("%d/%d"), MainCharacter->GetCurrentHP(), MainCharacter->GetMaxHP()),
                   FString::FromInt(MainCharacterStatusComponent->GetHealthSpecialty()),
                   [&]()
                   {
                       MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Health);
                   });

    AddContentItem(TEXT("灵气"),
                   FString::Printf(TEXT("%d/%d"), MainCharacter->GetCurrentSP(), MainCharacter->GetMaxSP()),
                   FString::FromInt(MainCharacterStatusComponent->GetStaminaSpecialty()),
                   [&]()
                   {
                       MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Stamina);
                   });

    AddContentItem(TEXT("灵气强度"),
                   MainCharacterStatusComponent->GetAttack(),
                   MainCharacterStatusComponent->GetAttackSpecialty(),
                   [&]()
                   {
                       MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Attack);
                   });
    AddContentItem(TEXT("护体灵气"),
                   MainCharacterStatusComponent->GetDefense(),
                   MainCharacterStatusComponent->GetDefenseSpecialty(),
                   [&]()
                   {
                       MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Defense);
                   });

    AddContentItem(TEXT("韧性"),
                   MainCharacterStatusComponent->GetToughness(),
                   MainCharacterStatusComponent->GetToughnessSpecialty(),
                   [&]()
                   {
                       MainCharacter->GetCharacterStatusComponent()->LevelUp(ESpecialties::Toughness);
                   });
}

void UCharacterPropertiesPage::AddContentItem(FString Name, FString Value, FString Speciality,
                                              const std::function<void()>& ClickEffect)
{
    FGamingMenuContentItemStruct GamingMenuContentItemStruct = {
        FText::FromString(Name),
        FText::FromString(Value),
        FText::FromString(Speciality),
        ClickEffect
    };
    Super::AddContentItem(CharacterPropertiesPageItemWidgetClass, std::move(GamingMenuContentItemStruct));
}

void UCharacterPropertiesPage::AddContentItem(FString Name, int Value, int Speciality,
                                              const std::function<void()>& ClickEffect)
{
    FGamingMenuContentItemStruct GamingMenuContentItemStruct = {
        FText::FromString(Name),
        FText::AsNumber(Value),
        FText::AsNumber(Speciality),
        ClickEffect
    };
    Super::AddContentItem(CharacterPropertiesPageItemWidgetClass, std::move(GamingMenuContentItemStruct));
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
        FGamingMenuContentItemStruct GamingMenuContentItemStruct = {
            TranscendentalLawsSystem->GetDisplayName(),
            FText::FromName(TranscendentalLawsSystem->GetCategory()),
            TranscendentalLawsSystem->
            GetTranscendentalLawsAttainmentText(),
            []()
            {
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(
                        -1, 5, FColor::Yellow,TEXT("提示仙道信息"));
                }
            }
        };
        AddContentItem(CharacterTranscendentalLawsSystemPageItemWidgetClass, std::move(GamingMenuContentItemStruct));
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
