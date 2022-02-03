// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthWidget.h"
#include "../Components/UHealthComponent.h"
#include "../Pawns/TankChar.h"
#include "Components/TextBlock.h"
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

	UUHealthComponent* PlayerHealthComponent = Cast<UUHealthComponent>(PlayerPawn->GetComponentByClass(UUHealthComponent::StaticClass()));
	if (!IsValid(PlayerHealthComponent))
	{
		UE_LOG(TTLogHealthWidget, Error, TEXT("Unable to get Health Component reference"));
	}

	PlayerHealthComponent->OnPlayerHit.AddUniqueDynamic(this, &UHealthWidget::UpdateHealthPercent);
	UpdateHealthPercent();
}

void UHealthWidget::UpdateHealthPercent()
{

	ATankChar* PlayerPawn = GetOwningPlayerPawn<ATankChar>();
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(TTLogHealthWidget, Error, TEXT("Unable to get player pawn"));
	}

	UUHealthComponent* PlayerHealthComponent = Cast<UUHealthComponent>(PlayerPawn->GetComponentByClass(UUHealthComponent::StaticClass()));
	if (!IsValid(PlayerHealthComponent))
	{
		UE_LOG(TTLogHealthWidget, Error, TEXT("Unable to get Health Component reference"));
	}

	float CurrentHealthPercent = PlayerHealthComponent->Health;

	HealthNumber->SetText(FText::AsNumber(CurrentHealthPercent));
	HealthProgressBar->SetPercent(CurrentHealthPercent / 100);

	InvalidateLayoutAndVolatility();
}
