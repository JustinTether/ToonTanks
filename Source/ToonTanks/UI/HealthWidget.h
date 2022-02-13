// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <GameplayEffectTypes.h>
#include "HealthWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UProgressBar;

DECLARE_LOG_CATEGORY_EXTERN(TTLogHealthWidget, Log, All);

UCLASS()
class TOONTANKS_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthNumber;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	void UpdateHealthPercent(const FOnAttributeChangeData& ChangedAttribute);

	void SetInitialValue(float InitialValue);

	void RemoveWidget();

private:

protected:
	void NativeConstruct() override;
	
};
