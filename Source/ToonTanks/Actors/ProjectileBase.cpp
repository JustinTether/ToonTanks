// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsModule.h"
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

	AbilitySystemComponent = CreateDefaultSubobject<UTTAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}


// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

}

class UAbilitySystemComponent* AProjectileBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpuse, const FHitResult& Hit)
{
	AActor* ProjectileOwner = GetOwner();

	if (!IsValid(ProjectileOwner))
	{
		Destroy();
		return;
	}

	if (OtherActor == ProjectileOwner)
	{
		Destroy();
		return;
	}

	if (OtherActor == this)
	{
		return;
	}

	if (!DefaultDamageEffect)
	{
		Destroy();
		return;
	}

	//Grab GameplayAbilitySystem of actor and apply GameplayEffect
	ATankChar* TankToHit;
	TankToHit = Cast<ATankChar>(OtherActor);

	if (!IsValid(TankToHit))
	{
		Destroy();
		return;
	}

	UTTAbilitySystemComponent* TankAbilitySystem;
	TankAbilitySystem = Cast<UTTAbilitySystemComponent>(TankToHit->GetComponentByClass(UTTAbilitySystemComponent::StaticClass()));

	if (!IsValid(TankAbilitySystem))
	{
		Destroy();
		return;
	}

	FGameplayEffectContextHandle EffectContext = TankAbilitySystem->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultDamageEffect, 1, EffectContext);

	if (!EffectHandle.IsValid())
	{
		Destroy();
		return;
	}

	FActiveGameplayEffectHandle ActiveHandle = TankAbilitySystem->ApplyGameplayEffectSpecToSelf(*EffectHandle.Data.Get());
	Destroy();
}

