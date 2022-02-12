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

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UTTAttributeSet, Health);


	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

private:


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
