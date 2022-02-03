// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "TurretPawn.generated.h"

class ATankPawn;
UCLASS()
class TOONTANKS_API ATurretPawn : public ATankBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretAI", meta = (AllowPrivateAccess = "True"))
	float FireRate = 2.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TurretAI", meta = (AllowPrivateAccess = "True"))
	float TurretSightRange = 500.f;

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void DestroyPawn() override;

private:
	void CanFire();
	void AimTurret();
	float DistanceToPlayer();


	FTimerHandle FireRateTimer;
	ATankPawn* Player = nullptr;
	

	
	
};
