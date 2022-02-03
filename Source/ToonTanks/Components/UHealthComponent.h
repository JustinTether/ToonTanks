// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerHitDelegate);

	// Sets default values for this component's properties
	UUHealthComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Player")
	FOnPlayerHitDelegate OnPlayerHit;

	UFUNCTION()
	void OnRep_HealthChanged();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, ReplicatedUsing = OnRep_HealthChanged, Category="Player")
	float Health;

	UPROPERTY(EditAnywhere)
	float DefaultHealth;

	
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);



private:
	
	AToonTanksBase* GameMode;

		
};
