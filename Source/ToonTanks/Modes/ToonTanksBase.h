// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ToonTanksBase.generated.h"

class ATTGameState;
class APawn;

UCLASS()
class TOONTANKS_API AToonTanksBase : public AGameModeBase
{
	GENERATED_BODY()
	

private:
	

public:
	void PlayerHit();
	void ActorDied(AActor* DeadActor, AActor* Killer);

/*	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ActorDied_Server(AActor* DeadActorRep, AActor* Killer); */

	UFUNCTION(BlueprintCallable)
	void SpawnNewTank(AController* PlayerController);

protected:
	AToonTanksBase();

	void DisableMoveInput(APawn* TankPawn);
	void EnableMoveInput(APawn* TankPawn);

	ATTGameState* GameState;

	int HitCount = 0;

	UPROPERTY(EditAnywhere)
	int MaxScore = 0;

	UPROPERTY(EditAnywhere)
	int MinPlayers = 0;

	UPROPERTY(EditAnywhere)
	float PreRoundTime;

	UPROPERTY(EditAnywhere)
	float MaxRoundTime;

	UPROPERTY(EditAnywhere)
	float RoundsPerMatch;

	FTimerHandle PreRoundTimer;
	FTimerHandle RoundTimer;
	FTimerHandle EndMatchTimer;

	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	//UFUNCTION(BlueprintNativeEvent, Category = Game)
	//virtual AActor* FindPlayerStart(AController* Player, const FString& IncomingName = TEXT(""));

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION()
	void HandlePreRound();

	UFUNCTION()
	void HandleGameOver(bool playerWon);

	UFUNCTION()
	void HandleRoundStart();

	UFUNCTION()
	void HandleRoundEnd();

	UFUNCTION()
	void HandleMatchEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool PlayerWon); //TODO: Change for when we do multiplayer

	/* Implementing finding player starts based on TeamID.*/
	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = TEXT(""));

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual int32 GetNumPlayers() override;

	UFUNCTION()
	bool RespawnTank(AController* PlayerController);
	bool HandleTankDeath(AController* PlayerController, bool bShouldRespawn);


};
