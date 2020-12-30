// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UMG/Public/Components/HorizontalBox.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ARPGUISubsystem.generated.h"

class AARPGPlayerController;
class AARPGCharacter;
class UARPGPageWidget;
class UARPGTabWidget;
class UARPGPageContentItemWidget;
class UARPGTitleWidget;
class UARPGButton;

/**
 * 管理游戏中的所有UI的子系统
 */
UCLASS()
class UARPGUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


	UPROPERTY()
	TArray<UARPGPageWidget*> UserWidgetsStack;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:

	virtual TArray<UARPGPageWidget*>& GetUserWidgetsStack()
	{
		return UserWidgetsStack;
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FButtonClickEvent, UARPGButton*);
/*对Button进行简单封装，增加具有this参数的静态委托，让外界得知被点击的具体是哪个Button*/
UCLASS()
class UARPGButton : public UButton
{
	GENERATED_BODY()

	FButtonClickEvent ButtonClickEvent;

	UFUNCTION()
	void BindToOnClick() { ButtonClickEvent.Broadcast(this); };
public:
	UARPGButton() { OnClicked.AddDynamic(this, &UARPGButton::BindToOnClick); };

	virtual FButtonClickEvent& OnButtonClick() { return ButtonClickEvent; }
};

UCLASS()
class UARPGWidget : public UUserWidget
{
	GENERATED_BODY()

	TWeakObjectPtr<AARPGCharacter> OwnerCharacter;
	TWeakObjectPtr<AARPGPlayerController> OwnerPlayerController;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	virtual void Show()
	{
		Refresh();
		SetVisibility(ESlateVisibility::Visible);
	}

	virtual FText GetDisplayName() { return {}; }

	virtual void Hide() { SetVisibility(ESlateVisibility::Hidden); }

	virtual void Erase() { ; }

	virtual void Refresh() { ; }


	virtual AARPGCharacter* GetOwnerCharacter()
	{
		return OwnerCharacter.Get();
	}

	virtual void SetOwnerCharacter(AARPGCharacter* NewOwnerCharacter);

	virtual AARPGPlayerController* GetOwnerPlayerController()
	{
		return OwnerPlayerController.Get();
	}

	virtual void SetOwnerPlayerController(AARPGPlayerController* NewOwnerPlayerController);
};

USTRUCT(BlueprintType)
struct FTabWidgetPageConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="TabWidgetPageConfig")
	FText TitleName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="TabWidgetPageConfig")
	UTexture2D* TitleIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="TabWidgetPageConfig")
	TSubclassOf<UARPGPageWidget> PageClass;
};


/*标签式菜单页父类，使用该类需设置TitleClass指定标题栏的风格，在TabWidgetPageConfigs配置所有标签页*/
UCLASS()
class UARPGTabWidget : public UARPGWidget
{
	GENERATED_BODY()

	UPROPERTY()
	UARPGPageWidget* CurrentActiveWidget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UHorizontalBox* HorizontalBox_TitleBar;

	UPROPERTY(BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(AllowPrivateAccess))
	TMap<UARPGButton*, UARPGPageWidget*> TabButtonsMap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(AllowPrivateAccess))
	TSubclassOf<UARPGTitleWidget> TitleClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(AllowPrivateAccess))
	TArray<FTabWidgetPageConfig> TabWidgetPageConfigs;
protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;
public:

	void AddPage(UARPGButton* TriggerButton, UARPGPageWidget* PageWidget);


	virtual UARPGPageWidget* GetCurrentActiveWidget()
	{
		return CurrentActiveWidget;
	}

	virtual void SetCurrentActiveWidget(UARPGPageWidget* NewCurrentActiveWidget)
	{
		this->CurrentActiveWidget = NewCurrentActiveWidget;
	}
};


UCLASS()
class UARPGPageWidget : public UARPGWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UScrollBox* ScrollBox_Content;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(AllowPrivateAccess))
	TSubclassOf<UARPGPageContentItemWidget> ItemWidgetClass;
protected:
	virtual void NativeConstruct() override;

	/*重载此方法返回内容，例如角色所有的属性构建的一组ContentItemWidget*/
	virtual TArray<UARPGPageContentItemWidget*> SetupContent() { return TArray<UARPGPageContentItemWidget*>{}; }
public:
	virtual void Show() override;

	virtual void Hide() override;

	virtual UScrollBox* GetScrollBox_Content()
	{
		return ScrollBox_Content;
	}

	virtual TSubclassOf<UARPGPageContentItemWidget> GetItemWidgetClass()
	{
		return ItemWidgetClass;
	}


	virtual void SetItemWidgetClass(TSubclassOf<UARPGPageContentItemWidget> NewItemWidgetClass)
	{
		this->ItemWidgetClass = NewItemWidgetClass;
	}
};

UCLASS()
class UARPGPageContentItemWidget : public UARPGWidget
{
	GENERATED_BODY()
};

UCLASS()
class UARPGSingleButtonItemWidget : public UARPGPageContentItemWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UTextBlock* TextBlock_ItemName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UTextBlock* TextBlock_ItemValue;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UTextBlock* TextBlock_ItemOperation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UARPGButton* Button_Operation;

public:
	void SetItemName(FText ItemName) { TextBlock_ItemName->SetText(ItemName); }
	void SetItemValue(FText ItemValue) { TextBlock_ItemValue->SetText(ItemValue); }
	void SetItemOperationText(FText ItemOperationText){TextBlock_ItemOperation->SetText(ItemOperationText);};

	virtual UARPGButton* GetOperationButton() { return Button_Operation; }
};

UCLASS()
class UARPGTitleWidget : public UARPGWidget
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UTextBlock* TextBlock_TitleName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UARPGButton* Button_TriggerPageSwitch;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="ARPGGameSettingsWidget",meta=(BindWidget,AllowPrivateAccess))
	UImage* Image_TitleIcon;
protected:
	virtual bool Initialize() override;

	virtual void NativePreConstruct() override;
public:
	void SetTitleName(FText TitleName) { TextBlock_TitleName->SetText(TitleName); };
	void SetTitleIcon(UTexture2D* TitleIcon) { Image_TitleIcon->SetBrushFromTexture(TitleIcon); };

	virtual UARPGButton* GetTriggerPageSwitchButton()
	{
		return Button_TriggerPageSwitch;
	}
};
