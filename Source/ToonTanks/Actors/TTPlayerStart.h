// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "TTPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATTPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	int TeamID = -1;

protected:
};
