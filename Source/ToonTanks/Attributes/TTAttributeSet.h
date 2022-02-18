// Copyright Justin Tether

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "TTAttributeSet.generated.h"

/**
 * 
 */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TOONTANKS_API UTTAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UTTAttributeSet();

	/* Tank Health */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTTAttributeSet, Health);


	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	/* Movement Speed */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UTTAttributeSet, MovementSpeed);

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

	/* Fire Delay for Turret */
	UPROPERTY(BlueprintReadOnly, Category = "Turret", ReplicatedUsing = OnRep_FireDelay)
	FGameplayAttributeData FireDelay;
	ATTRIBUTE_ACCESSORS(UTTAttributeSet, FireDelay);

	UFUNCTION()
	void OnRep_FireDelay(const FGameplayAttributeData& OldFireDelay);

	/* Projectile Speed */
	UPROPERTY(BlueprintReadOnly, Category = "Turret", ReplicatedUsing = OnRep_ProjectileSpeed)
	FGameplayAttributeData ProjectileSpeed;

	ATTRIBUTE_ACCESSORS(UTTAttributeSet, ProjectileSpeed);

	UFUNCTION()
		void OnRep_ProjectileSpeed(const FGameplayAttributeData& OldProjectileSpeed);

private:


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
