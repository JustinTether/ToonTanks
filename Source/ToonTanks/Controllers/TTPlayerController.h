// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTPlayerController.generated.h"

class AToonTanksBase;

UCLASS()
class TOONTANKS_API ATTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSpawned);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDied);

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnPlayerSpawned OnPlayerSpawned;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnPlayerDied OnPlayerDied;

	AToonTanksBase* GameMode;

	UFUNCTION()
	void OnRep_TeamID();

	UPROPERTY(BlueprintReadWrite, Replicated, ReplicatedUsing = OnRep_TeamID)
	int TeamID;

	UFUNCTION()
	void OnRep_bIsSpawned();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_bIsSpawned)
	bool bIsSpawned = false;

	UFUNCTION()
	void OnRep_bIsDead();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_bIsDead)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	void SpawnNewTank(ATTPlayerController* TankController, int Team);

	void SetTextureByTeam_Server(ATTPlayerController* TankController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnNewTank_Server(ATTPlayerController* TankController, int Team);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void HandleLeaveSession();

private:


protected:
	ATTPlayerController();

	virtual void BeginPlay() override;

	/* Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnPossess(APawn* InPawn) override;
};
