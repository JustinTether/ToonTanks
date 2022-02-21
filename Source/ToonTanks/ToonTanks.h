// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Online.h"

UENUM(BlueprintType)
enum class TTAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Fire,
};