// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



#include "TranscendentalCombatComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "functional"



#include "ARPGGamingMenuWidget.generated.h"


class UARPGGamingMenuPageWidget;
class UARPGGamingMenuContentItemWidget;
class UGamingMenuTitleItemWidgetConfig;


/**
 * 
 */
UCLASS()
class TESTPROJECT_API UARPGGamingMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget))
    UHorizontalBox* HorizontalBox_TitleOptions;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget")
    UARPGGamingMenuPageWidget* CurrentActiveMenuPage;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget))
    UScrollBox* ScrollBox_Content;

    virtual bool Initialize() override;

    virtual void NativeConstruct() override;
};

USTRUCT()
struct FGamingMenuContentItemStruct
{
    GENERATED_BODY()

    FText LeftText;
    FText MiddleText;
    FText ButtonText;
    std::function<void()> ButtonClickEffect;
};

UCLASS()
class TESTPROJECT_API UARPGGamingMenuPageWidget : public UUserWidget
{
    GENERATED_BODY()
protected:

    
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuTitleItemWidget",meta=(BindWidget))
    UButton* Button_Option;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuTitleItemWidget",meta=(BindWidget))
    UTextBlock* TextBlock_PageTitleName;

    UPROPERTY(EditDefaultsOnly,Category="ARPGGamingMenuTitleItemWidget")
    TSubclassOf<UARPGGamingMenuContentItemWidget> GamingMenuContentItemWidgetClass;

    

public:
    virtual bool Initialize() override;
    
    virtual void NativeConstruct() override;

    virtual FText GetPageTitleName(){return FText::FromString(TEXT("设置"));};

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget")
    UScrollBox* ScrollBox_Content;

    UARPGGamingMenuWidget* ParentMenuWidget;

    UFUNCTION(BlueprintImplementableEvent,Category="ARPGGamingMenuTitleItemWidget")
    void BeSelected();

    UFUNCTION(BlueprintImplementableEvent,Category="ARPGGamingMenuTitleItemWidget")
    void NotSelected();

    virtual void NativeBeSelected();

    virtual void NativeNotSelected();

    DECLARE_DELEGATE_OneParam(FClickTitleOptionEvent,UARPGGamingMenuPageWidget*);
    FClickTitleOptionEvent OnClickTitleOption;

    UFUNCTION()
    void Button_OptionOnClick();

    UARPGGamingMenuContentItemWidget* AddContentItem(TSubclassOf<UARPGGamingMenuContentItemWidget> PageItemWidgetClass,
                                                 const FGamingMenuContentItemStruct& GamingMenuContentItemStruct);

    virtual void AddSeparator(TSubclassOf<UARPGGamingMenuContentItemWidget> ContentSeparatorWidgetClass,
                                             FText LeftText, FText MiddleText, FText RightText);
};



UCLASS()
class TESTPROJECT_API UARPGGamingMenuContentItemWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuContentItemWidget",meta=(BindWidget))
    UTextBlock* TextBlock_LeftText;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuContentItemWidget",meta=(BindWidget))
    UTextBlock* TextBlock_MiddleText;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuContentItemWidget",meta=(BindWidget))
    UTextBlock* TextBlock_Button1Text;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGamingMenuContentItemWidget",meta=(BindWidget))
    UButton* Button_1;

    virtual bool Initialize() override;

    DECLARE_DELEGATE(FButtonCLickEvent);
    FButtonCLickEvent ClickButton1;
    
    UFUNCTION()
    void OnClickButton1(){ClickButton1.ExecuteIfBound();};
};

UCLASS()
class UCharacterPropertiesPage : public UARPGGamingMenuPageWidget
{
    GENERATED_BODY()
    
    class AARPGMainCharacter* MainCharacter;
    class UCharacterStatusComponent* MainCharacterStatusComponent;
    
public:
    virtual FText GetPageTitleName() override{return FText::FromString(TEXT("状态"));}
    
    virtual void NativeConstruct() override;

    virtual void NativeBeSelected() override;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterPropertiesPage")
    TSubclassOf<UARPGGamingMenuContentItemWidget> CharacterPropertiesPageItemWidgetClass;

    void AddContentItem(FString Name,FString Value,FString Speciality,const std::function<void()>& ClickEffect);
    void AddContentItem(FString Name,int Value,int Speciality,const std::function<void()>& ClickEffect);
};

UCLASS()
class UCharacterTranscendentalLawsSystemPage : public UARPGGamingMenuPageWidget
{
    GENERATED_BODY()

    class AARPGMainCharacter* MainCharacter;
    class UTranscendentalCombatComponent* MainCharacterCombatComponent;
public:
    virtual FText GetPageTitleName() override{return FText::FromString(TEXT("仙道"));}
    
    virtual void NativeConstruct() override;
    virtual void NativeBeSelected() override;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="CharacterPropertiesPage")
    TSubclassOf<UARPGGamingMenuContentItemWidget> CharacterTranscendentalLawsSystemPageItemWidgetClass;
};

UCLASS()
class UCharacterReturnToGamePage : public UARPGGamingMenuPageWidget
{
    GENERATED_BODY()

public:
    virtual FText GetPageTitleName() override{return FText::FromString(TEXT("返回"));}
    
    virtual void NativeBeSelected() override;
};

UCLASS()
class UCharacterQuitGamePage : public UARPGGamingMenuPageWidget
{
    GENERATED_BODY()

public:
    virtual FText GetPageTitleName() override{return FText::FromString(TEXT("退出"));}
    
    virtual void NativeBeSelected() override;
};

