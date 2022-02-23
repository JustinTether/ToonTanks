
//TT Includes
#include "TTAbilityPickup.h"

//UE4 Includes
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

ATTAbilityPickup::ATTAbilityPickup()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision Capsule"));
	CapsuleComponent->SetupAttachment(RootComponent);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ATTAbilityPickup::OnHit);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotating Movement Component"));
	RotatingMovementComponent->bAutoActivate = true;
	SetPickupState(TTPickupState::ACTIVE);
	RechargeTimer = 25.0f;
	EffectDuration = 10.0f;
}

void ATTAbilityPickup::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState != TTPickupState::ACTIVE)
	{
		return;
	}

	if (!PickupEffect)
	{
		return;
	}

	if (!IsValid(OtherActor))
	{
		return;
	}


	ATankChar* PlayerTank;
	PlayerTank = Cast<ATankChar>(OtherActor);
	if (!IsValid(PlayerTank))
	{
		return;
	}

	UTTAbilitySystemComponent* TankAbilitySystem;
	TankAbilitySystem = Cast<UTTAbilitySystemComponent>(OtherActor->GetComponentByClass(UTTAbilitySystemComponent::StaticClass()));
	
	if (!IsValid(TankAbilitySystem))
	{
		return;
	}
	UGameplayEffect* AppliedGE = PickupEffect.GetDefaultObject();
	AppliedGE->DurationMagnitude = FScalableFloat(EffectDuration);

	
	TankAbilitySystem->ApplyGameplayEffectToSelf(AppliedGE, 1.0f, TankAbilitySystem->MakeEffectContext());
	SetPickupState(TTPickupState::RECHARGING);
}

void ATTAbilityPickup::SetPickupState(TTPickupState NewState)
{
	switch (NewState)
	{
		case TTPickupState::ACTIVE:
			CurrentState = NewState;
			StaticMeshComponent->SetHiddenInGame(false);
			break;

		case TTPickupState::DISABLED:
			CurrentState = NewState;
			StaticMeshComponent->SetHiddenInGame(true);
			break;

		case TTPickupState::RECHARGING:
			CurrentState = NewState;
			StaticMeshComponent->SetHiddenInGame(true);
			FTimerHandle RechargeTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(RechargeTimerHandle, FTimerDelegate::CreateUObject(this, &ATTAbilityPickup::SetPickupState, TTPickupState::ACTIVE), RechargeTimer, false);
			break;
	}
}