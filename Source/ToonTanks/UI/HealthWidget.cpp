// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "../Components/UHealthComponent.h"
#include "../Pawns/TankChar.h"
#include "Components/TextBlock.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "Components/ProgressBar.h"

DEFINE_LOG_CATEGORY(TTLogHealthWidget);

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Grab Health Component, bind to the player's Pawn

	ATankChar* PlayerPawn = GetOwningPlayerPawn<ATankChar>();
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(TTLogHealthWidget, Error, TEXT("Unable to get player pawn"));
	}

	//Instead of binding to the OnHit let's bind to the GameplayAttributeValueChangeDelegate
	UTTAbilitySystemComponent* PlayerAbilitySystem;

	PlayerAbilitySystem = Cast<UTTAbilitySystemComponent>(PlayerPawn->GetComponentByClass(UTTAbilitySystemComponent::StaticClass()));

	if (IsValid(PlayerAbilitySystem))
	{
		PlayerAbilitySystem->GetGameplayAttributeValueChangeDelegate(PlayerPawn->Attributes->GetHealthAttribute()).AddUObject(this, &UHealthWidget::UpdateHealthPercent);
		float StartingHealth = PlayerPawn->Attributes->Health.GetCurrentValue();
		SetInitialValue(StartingHealth);
	}
}


void UHealthWidget::SetInitialValue(float InitialValue)
{
	HealthNumber->SetText(FText::AsNumber(InitialValue));
	HealthProgressBar->SetPercent(InitialValue / 100);

	InvalidateLayoutAndVolatility();
}

void UHealthWidget::UpdateHealthPercent(const FOnAttributeChangeData& ChangedAttribute)
{
	float CurrentHealthPercent = ChangedAttribute.NewValue;
	HealthNumber->SetText(FText::AsNumber(CurrentHealthPercent));
	HealthProgressBar->SetPercent(CurrentHealthPercent / 100);

	InvalidateLayoutAndVolatility();
}
