
#pragma once
//UE4 Includes


//Project Includes
#include "ToonTanks/ToonTanks.h"
#include "Abilities/GameplayAbility.h"
#include "TTGameplayAbility.generated.h"

UCLASS()
class UTTGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UTTGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TTAbilityInputID AbilityInputID = TTAbilityInputID::None;

private:

protected:

};