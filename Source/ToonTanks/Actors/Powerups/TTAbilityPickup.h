#pragma once

//UE4 Includes
#include "GameFramework/Actor.h"
#include <GameplayEffectTypes.h>


//TTIncludes
#include "ToonTanks/Pawns/TankChar.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "TTAbilityPickup.generated.h"

UENUM(BlueprintType)
enum TTPickupState
{
	ACTIVE UMETA(DisplayName = "ACTIVE"),
	RECHARGING UMETA(DisplayName = "RECHARGING"),
	DISABLED UMETA(DisplayName = "DISABLED"),
};

class UCapsuleComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;

UCLASS()
class ATTAbilityPickup : public AActor
{
	GENERATED_BODY()

public:
	ATTAbilityPickup();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RechargeTimer;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UCapsuleComponent* CapsuleComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	URotatingMovementComponent* RotatingMovementComponent = nullptr;

	void SetPickupState(TTPickupState NewState);

protected:
	TTPickupState CurrentState;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> PickupEffect;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Abilities")
	float AbilityDuration;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};