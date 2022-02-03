// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Pawns/TankChar.h"
#include "../Controllers/TTPlayerController.h"



// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	InitialLifeSpan = 3.f;
}


// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpuse, const FHitResult& Hit)
{
	AActor* ProjectileOwner = GetOwner();

	if (OtherActor != ProjectileOwner)
	{
		if (IsValid(ProjectileOwner))
		{
			if (OtherActor && OtherActor != this)
			{
					UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, ProjectileOwner->GetInstigatorController(), this, DamageType);
			}
			Destroy();
		}
	}
	//If the projectile came from the owner just destroy the projectile
	else Destroy();
	
}

