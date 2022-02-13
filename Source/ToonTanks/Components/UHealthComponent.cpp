// Fill out your copyright notice in the Description page of Project Settings.

//Project Includes
#include "UHealthComponent.h"
#include "ToonTanks/Pawns/TankChar.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "ToonTanks/Modes/ToonTanksBase.h"

//UE4 Includes
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "GameplayEffectExtension.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UUHealthComponent::UUHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AToonTanksBase>(UGameplayStatics::GetGameMode(GetWorld()));
	//GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UUHealthComponent::ApplyDamage);
	//Bind to OnAttributedChanged, if the value is 0, call ActorDied
	ATankChar* PlayerPawn;
	PlayerPawn = Cast<ATankChar>(GetOwner());

	if (!IsValid(PlayerPawn))
	{
		return;
	}
	
	UTTAbilitySystemComponent* PlayerAbilityComponent;
	PlayerAbilityComponent = Cast<UTTAbilitySystemComponent>(PlayerPawn->GetAbilitySystemComponent());

	PlayerAbilityComponent->GetGameplayAttributeValueChangeDelegate(PlayerPawn->Attributes->GetHealthAttribute()).AddUObject(this, &UUHealthComponent::CheckActorDeath);
}

void UUHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UUHealthComponent, LastDamageDealer);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UUHealthComponent::CheckActorDeath(const FOnAttributeChangeData& ChangedAttribute)
{
	if (GetOwner()->HasAuthority())
	{
		float NewHealth = ChangedAttribute.NewValue;
		bool bIsDead = NewHealth <= 0;

		LastDamageDealer = ChangedAttribute.GEModData->EffectSpec.GetContext().GetInstigator();

		if (!bIsDead)
		{
			return;
		}

		if (GameMode)
		{
			GameMode->ActorDied(GetOwner(), LastDamageDealer);
		}
	}


}


