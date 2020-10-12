// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable CppMemberFunctionMayBeConst
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "ARPGWidgetsLab.generated.h"

/**
* 
*/
UCLASS()
class TESTPROJECT_API UARPGWidgetsLab : public UUserWidget
{
    GENERATED_BODY()
};

UCLASS()
class UARPGNotifyWidget : public UUserWidget
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UImage* Image_Icon;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UTextBlock* TextBlock_Title;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UTextBlock* TextBlock_Content;

protected:
    virtual bool Initialize() override;
public:
    void ShowNotify(UTexture2D* Icon, FText Title, FText Content);
};


UCLASS()
class UARPGProgressBar : public UUserWidget
{
    GENERATED_BODY()

    float TotalValue;
    float TrueValue;
    float DummyValue;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGProgressBar",meta=(BindWidget,AllowPrivateAccess))
    class UProgressBar* ProgressBar;

protected:
    virtual bool Initialize() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
    UFUNCTION(BlueprintCallable,Category="ARPGProgressBar")
    void SetPercent(int Current, int Total);
};


UCLASS()
class UARPGPromptWidget : public UUserWidget
{
    GENERATED_BODY()


    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UButton* Button_Yes;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UButton* Button_No;

    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    class UTextBlock* TextBlock_PromptText;

public:
    virtual bool Initialize() override;

    DECLARE_DELEGATE(FMakeDecisionEvent);
    FMakeDecisionEvent OnChooseYes;
    FMakeDecisionEvent OnChooseNo;

    UFUNCTION()
    void OnClickButton_Yes();
    
    UFUNCTION()
    void OnClickButton_No();

    void SetPromptText(FText PromptText);
};

UCLASS()
class UShowPromptWidgetBlueprintNode : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
    TSubclassOf<UARPGPromptWidget> PromptWidgetClass;

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMakeDecisionEvent);

    UPROPERTY(BlueprintAssignable)
    FMakeDecisionEvent Yes;

    UPROPERTY(BlueprintAssignable)
    FMakeDecisionEvent No;


    UFUNCTION(BlueprintCallable,Category="ARPGWidgetsLab",meta=(BlueprintInternalUseOnly="true",WorldContext=
        "WorldContextObject"))
    static UShowPromptWidgetBlueprintNode* ShowPromptWidget(const UObject* WorldContextObject,FText PromptText);

    void CreatePromptWidget(const UWorld* World, FText PromptMessage);
};

//TODO 考虑是否需要存档读档界面
//
// UCLASS()
// class UARPGArchiveWidget : public UUserWidget
// {
//     GENERATED_BODY()
//
//     UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
//     class UScrollBox* ScrollBox_Archives;
// };
//
//
// UCLASS()
// class UARPGArchiveItemWidget : public UUserWidget
// {
//     GENERATED_BODY()
//
//     UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
//     class UTextBlock* TextBlock_ArchiveName;
//
//     UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
//     class UTextBlock* TextBlock_ArchiveTimestamp;
// };

UCLASS()
class UARPGLockTargetWidget : public UUserWidget
{
    GENERATED_BODY()

    //锚点需选在左上角，x=0.5、y=0.5对其
    UPROPERTY(BlueprintReadOnly,Category="ARPGWidgetsLab",meta=(BindWidget,AllowPrivateAccess))
    UImage* Image_LockIcon;
    
public:
    virtual bool Initialize() override;

    void SetLockIconScreenPosition(FVector2D ScreenPosition);
};
