// Fill out your copyright notice in the Description page of Project Settings.


#include "UHealthComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "../Pawns/TankBase.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "../Modes/ToonTanksBase.h"


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

	Health = DefaultHealth;
	GameMode = Cast<AToonTanksBase>(UGameplayStatics::GetGameMode(GetWorld()));
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UUHealthComponent::ApplyDamage);
}

void UUHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UUHealthComponent, Health);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UUHealthComponent::OnRep_HealthChanged()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep Health changed new health: %f"), Health);
	OnPlayerHit.Broadcast();
}

void UUHealthComponent::ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (GetOwner()->HasAuthority())
	{
		if (Damage != 0)
		{
			Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
			UE_LOG(LogTemp, Error, TEXT("Calling UI NOTIFY FROM HEALTH COMPONENT: %s"), *GetOwner()->GetName());
			OnPlayerHit.Broadcast();
			if (Health <= 0)
			{
				if (GameMode)
				{
					GameMode->ActorDied(GetOwner(), DamageCauser);
				}
			}
		}
	}
		
}

