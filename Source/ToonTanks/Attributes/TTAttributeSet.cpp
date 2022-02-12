// Copyright Justin Tether


#include "TTAttributeSet.h"
#include "Net/UnrealNetwork.h"


UTTAttributeSet::UTTAttributeSet()
{

}

void UTTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UTTAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UTTAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UTTAttributeSet, Health, OldHealth);
}