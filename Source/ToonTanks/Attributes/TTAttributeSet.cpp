// Copyright Justin Tether


#include "TTAttributeSet.h"
#include "Net/UnrealNetwork.h"


UTTAttributeSet::UTTAttributeSet()
{

}

void UTTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UTTAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UTTAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UTTAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTTAttributeSet, Health, OldHealth);
}

void UTTAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTTAttributeSet, MovementSpeed, OldMovementSpeed);
}

void UTTAttributeSet::OnRep_FireDelay(const FGameplayAttributeData& OldFireDelay)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTTAttributeSet, MovementSpeed, OldFireDelay);
}