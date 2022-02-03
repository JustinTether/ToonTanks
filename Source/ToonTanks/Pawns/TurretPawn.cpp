// Fill out your copyright notice in the Description page of Project Settings.


#include "TurretPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "TankPawn.h"


void ATurretPawn::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(FireRateTimer, this, &ATurretPawn::CanFire, FireRate, true);
	Player = Cast<ATankPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATurretPawn::Tick(float DeltaTime)
{
	//Called every frame
	Super::Tick(DeltaTime);
	if (IsValid(Player))
	{
		//RotateTurret(Player->GetActorLocation());
	}

}

float ATurretPawn::DistanceToPlayer()
{
	if (Player) 
	{
		return FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	}
	

	return 0;
}


void ATurretPawn::CanFire()
{
	//Does the player exist?
	if (Player)
	{
		//Maybe check if player is dead
		//Is the player within range?

		if (DistanceToPlayer() <= TurretSightRange)
		{
			ATurretPawn::Fire();

		}

	}
	

	

	//Do we have LOS to the player?

	//If so, fire

}

void ATurretPawn::AimTurret()
{

}

void ATurretPawn::DestroyPawn()
{
	Super::DestroyPawn();
	Destroy();
}
