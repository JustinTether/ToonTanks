// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayEffectTypes.h>
#include "Components/ActorComponent.h"
#include "UHealthComponent.generated.h"

//Class declarations
class UDamageType;
class AToonTanksBase;
class AController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOONTANKS_API UUHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UUHealthComponent();

	UPROPERTY(EditAnywhere)
	float DefaultHealth;

	/*Reference of the last actor that dealt damage to us */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = "Game Mode")
	AActor* LastDamageDealer = nullptr;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CheckActorDeath(const FOnAttributeChangeData& ChangedAttribute);

private:
	
	AToonTanksBase* GameMode;

		
};
