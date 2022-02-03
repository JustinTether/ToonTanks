// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TTGameState.generated.h"

class AToonTanksBase;

USTRUCT(BlueprintType)
struct FTeam
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int TeamID = -1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int Score = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString PrintName = "";

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 RoundsWon = 0;

public:
	void ResetScore(void)
	{
		Score = 0;
	}
};


UENUM()
enum MatchState
{
	WaitingForPlayers UMETA(DisplayName = "WaitingForPlayers"),
	RoundStarted UMETA(DisplayName = "RoundStarted"),
	RoundEnded UMETA(DisplayName = "RoundEnded"),
	PreRound UMETA(DisplayName = "PreRound"),
	MainMenu UMETA(DisplayName = "MainMenu")
	
};


UCLASS()
class TOONTANKS_API ATTGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/*Delegates for events, used for binding BP widgets to code events */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPreRoundStarted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTeamInfoChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxScoreChanged);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundStarted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundEnded, FTeam, WinningTeam);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnded);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWatingForPlayers); //Trigger some UI telling the player we're waiting for the lobby to fill
	

	AToonTanksBase* GameMode;

	// Array of teams. Maybe there's 2 teams? 4? 6? We'll let the mode decide this.
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_CurrentTeamsUpdated, Category = "GameState")
	TArray<FTeam> CurrentTeams;

	UPROPERTY(BlueprintReadOnly)
	FTeam WinningTeam;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnTeamInfoChanged OnTeamInfoChanged;

	UPROPERTY(BlueprintReadOnly, Replicated)
	float RoundTimer;

	UPROPERTY(BlueprintReadOnly)
	int32 TotalRoundsPlayed;

	/* Match State and the delegates for UI and Server->Client replication */
	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_MatchState, Category="GameState")
	TEnumAsByte<MatchState> MatchState;

	/* Function for calling Event Delegate for UI components when MatchState changes */
	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnPreRoundStarted OnPreRoundStarted;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnMaxScoreChanged OnMaxScoreChanged;

	UPROPERTY(BlueprintAssignable, Category="GameState")
	FOnRoundEnded OnRoundEnded;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnMatchEnded OnMatchEnded;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnRoundStarted OnRoundStarted;

	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnWatingForPlayers OnWaitingForPlayers;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "GameState")
	int CurrentModeMaxScore = 0;

	/* OnRep for when the gamemode changes a team value, score or otherwise. */
	UFUNCTION()
	void OnRep_CurrentTeamsUpdated();

	void CurrentTeamsUpdated();

	void MatchStateUpdated();

	int TimeToRespawn;



	/* Constructor */
	ATTGameState();

private:


protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void Tick(float DeltaSeconds) override;
};