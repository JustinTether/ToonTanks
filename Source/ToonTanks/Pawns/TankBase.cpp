// Fill out your copyright notice in the Description page of Project Settings.


#include "TankBase.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Actors/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ATankBase::ATankBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create the needed components of our tank, the base, the turret and the spawn point for your bullet

	//Create a capsule component, set it as root. This capsule will be our collison
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComponent;

	//Create the base mesh component which will hold the Tank Mesh
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetIsReplicated(true);
	//BaseMesh->SetSimulatePhysics(true);

	//Createthe movement component
	BaseMovementComponent = CreateDefaultSubobject<UPawnMovementComponent>(TEXT("Tank MovementComponent"));

	//Create the turret mesh component, attach it to the base mesh (So they follow eachother)
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);
	TurretMesh->SetIsReplicated(true);

	//Projectile spawn point, self explanatory
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Tank Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
	
	//SetRole(ROLE_Authority);

}


void ATankBase::RotateTurret(float LookAt)
{
	if (IsLocallyControlled())
	{
		FRotator TurretRotation;
		TurretRotation.Yaw = LookAt;
		TurretMesh->AddRelativeRotation(TurretRotation);
		RotateTurret_Server(LookAt);
	}

}

void ATankBase::RotateTurret_Server_Implementation(float LookLocation)
{
	if (!IsLocallyControlled())
	{
		FRotator TurretRotation;
		TurretRotation.Yaw = LookLocation;
		TurretMesh->AddRelativeRotation(TurretRotation);
	}
}

void ATankBase::Fire()
{
	if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		//Spawn a projectile at ProjectileSpawnPoint location and Rotation with momentum towards Rotation Vector
		if (ProjectileClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Projectile Spawning"));
			AProjectileBase* NewProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			NewProjectile->SetOwner(this);
			Fire_Server();
		}
	}
}

void ATankBase::Fire_Server_Implementation()
{
	if (!IsLocallyControlled())
	{
		//Spawn a projectile at ProjectileSpawnPoint location and Rotation with momentum towards Rotation Vector
		if (ProjectileClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Projectile Spawning"));
			AProjectileBase* NewProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, ProjectileSpawnPoint->GetComponentLocation(), ProjectileSpawnPoint->GetComponentRotation());
			NewProjectile->SetOwner(this);
		}
	}
}

void ATankBase::DestroyPawn()
{
	//Play death effects (Particle, Anim, whatever)

	//If Turret -> Inform Gamemode of death -> Destroy()

	//If Tank, Inform Gamemode of Player death -> Hide() and Stop input
}


