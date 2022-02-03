// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGameState.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "../Modes/ToonTanksBase.h"
#include "../Modes/ToonTanksBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"

ATTGameState::ATTGameState()
{
	//Create initial teams and add them to Team array
	FTeam Team0, Team1;

	Team0.TeamID = 0;
	Team0.PrintName = TEXT("Red Team");
	Team1.TeamID = 1;
	Team1.PrintName = TEXT("Blue Team");

	CurrentTeams.Add(Team0);
	CurrentTeams.Add(Team1);
	TimeToRespawn = 3;

}

void ATTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ATTGameState, MatchState);
	DOREPLIFETIME(ATTGameState, CurrentTeams);
	DOREPLIFETIME(ATTGameState, CurrentModeMaxScore)
	DOREPLIFETIME(ATTGameState, RoundTimer);

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


/* When the player hits 'play' or the game starts. Maybe we should set up some structure or functions for creating teams instead of doing this on the constructor.*/
void ATTGameState::BeginPlay()
{
	Super::BeginPlay();
	GameMode = Cast<AToonTanksBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATTGameState::MatchStateUpdated()
{
	//Event decleration for UI components
	//Match Start, Match Over triggered using this

	if (MatchState == MatchState::WaitingForPlayers)
	{
		OnWaitingForPlayers.Broadcast(); //Trigger UI Component for waiting for players
	}

	if (MatchState == MatchState::PreRound)
	{
		OnPreRoundStarted.Broadcast(); ///TODO: Change this to a round-start specific delegate
	}

	if (MatchState == MatchState::RoundStarted)
	{
		OnRoundStarted.Broadcast(); ///TODO: Change this to a round-start specific delegate
	}

	if (MatchState == MatchState::RoundEnded)
	{
		//Handle restarting the round, adding win to the team that has hit score limit
		OnRoundEnded.Broadcast(WinningTeam);
	}


}

void ATTGameState::OnRep_MatchState()
{
	//Event decleration for UI components
	//Match Start, Match Over triggered using this

	if (MatchState == MatchState::WaitingForPlayers)
	{
		OnWaitingForPlayers.Broadcast(); //Trigger UI Component for waiting for players
	}

	if (MatchState == MatchState::PreRound)
	{
		OnPreRoundStarted.Broadcast(); ///TODO: Change this to a round-start specific delegate
	}

	if (MatchState == MatchState::RoundStarted)
	{
		OnRoundStarted.Broadcast(); ///TODO: Change this to a round-start specific delegate
	}

	if (MatchState == MatchState::RoundEnded)
	{
		//Handle restarting the round, adding win to the team that has hit score limit
		OnRoundEnded.Broadcast(WinningTeam);
	}

	
}

/* The game mode will alter the team scores on confirmed kills, we'll use this on_rep to call the clients UI notify for updating team scores and other stuff */
void ATTGameState::OnRep_CurrentTeamsUpdated()
{
	OnTeamInfoChanged.Broadcast();
}

void ATTGameState::CurrentTeamsUpdated()
{
	OnTeamInfoChanged.Broadcast();
}

