// Fill out your copyright notice in the Description page of Project Settings.


#include "ToonTanksBase.h"
#include "../Pawns/TankChar.h"
#include "../Pawns/TurretPawn.h"
#include "../Actors/TTGameState.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "../Actors/TTPlayerStart.h"
#include "Engine/EngineTypes.h"
#include "../Controllers/TTPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"


AToonTanksBase::AToonTanksBase()
{
	//Set up the game as TDM with a maximum score and minimum players
	bStartPlayersAsSpectators = true;
	MaxScore = 5;
	MinPlayers = 2;
	PreRoundTime = 20.f;
	MaxRoundTime = 180.f;
	RoundsPerMatch = 3;

	PrimaryActorTick.bCanEverTick = true;
}


void AToonTanksBase::BeginPlay()
{
	GameState = GetWorld()->GetGameState<ATTGameState>();
	if (GetWorld()->GetName() == "MainMenu")
	{
		GameState->MatchState = MatchState::MainMenu;
		GameState->MatchStateUpdated();
		Super::BeginPlay();
	}
	else
	{
		//AToonTanksBase::HandlePreRound();
		GameState->MatchState == MatchState::WaitingForPlayers;
		GameState->MatchStateUpdated();
		GameState->RoundTimer = PreRoundTime; //Set PreRoundTime for countdown
		Super::BeginPlay();
	}
	
}

void AToonTanksBase::Tick(float DeltaSeconds)
{
	//If we're in pre-round, check if we meet the minimum player count

	if (GameState->MatchState == MatchState::WaitingForPlayers)
	{
		//Check the player count against MinPlayers
		if (GetNumPlayers() >= MinPlayers)
		{
			UE_LOG(LogTemp, Error, TEXT("Minimum Players Reached, starting round"));
			GameState->MatchState = MatchState::PreRound;
			GameState->MatchStateUpdated();
			HandlePreRound();
		}
		else
		{
			if (GameState->MatchState == MatchState::MainMenu) return;
			//UE_LOG(LogTemp, Error, TEXT("Current Players less than Minimum Players, Waiting for more. Current Players: %i"), GetNumPlayers());
			//UE_LOG(LogTemp, Error, TEXT("Current number of Spectators: %i"), GetNumSpectators())

		}
	}

	if (GameState->MatchState == MatchState::PreRound)
	{
		//Set the PreRound timer to the time remaining
		GameState->RoundTimer = GetWorldTimerManager().GetTimerRemaining(PreRoundTimer);
	}

	if (GameState->MatchState == MatchState::RoundStarted)
	{
		GameState->RoundTimer = GetWorldTimerManager().GetTimerRemaining(RoundTimer);
		//Check the current score
		for (auto Team : GameState->CurrentTeams)
		{
			if (Team.Score == MaxScore)
			{
				GameState->MatchState = MatchState::RoundEnded;
				GameState->MatchStateUpdated();
				HandleRoundEnd();
			}
		}
	}

	if (GameState->MatchState == MatchState::RoundEnded)
	{
		//Restart the gamemode, tally one winner, pass winner to round over delegate (For scoring)
		
		/*Check GameState->CurrentTeams for whichever team has the higher score (Maybe rounds can end at the end of the timer itself)
		* If any teams have won, increment round score for the teams and then run the end-round UI delegate, should show both teams (And their colours?) and current scores
		* After X seconds, reset team scores and respawn/lock all players for 10 seconds counting down (Create timer functionality and UI element)
		* Then release, enable input and start the round. Pre-Round should handle all of the spawning/locking players in their respective locations for the time alotment*/
		GameState->RoundTimer = GetWorldTimerManager().GetTimerRemaining(RoundTimer);

	}
}

void AToonTanksBase::HandlePreRound()
{
	GameState->MatchState = MatchState::PreRound;
	GameState->MatchStateUpdated();
	GameState->CurrentModeMaxScore = MaxScore;
	GameState->WinningTeam = FTeam();

	//Reset current team scores
	for (int i = 0; i < GameState->CurrentTeams.Num(); i++)
	{
		GameState->CurrentTeams[i].ResetScore();
	}

	//Spawn all players in their respective spawn positions, lock inputs, start countdown timer based on PreRoundTime or similar variable (Get GameState to trigger UI for this) 
	//Then once timer is complete fire HandleStartRound which will unlock all player inputs and swap MatchState to ::RoundStarted
	TArray<AActor*> AllPlayerControllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), AllPlayerControllers);

	for (auto PlayerController : AllPlayerControllers)
	{
		if (APlayerController* TankController = Cast<APlayerController>(PlayerController))
		{
			bool bWasSuccessful = RespawnTank(TankController);

			if (bWasSuccessful)
			{
				if (APawn* TankPawn = TankController->GetPawn())
				{
					DisableMoveInput(TankPawn);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("NO PAWN TO DISABLE INPUT FOR"));
				}
			}
		}

	}


	GetWorld()->GetTimerManager().SetTimer(PreRoundTimer, this, &AToonTanksBase::HandleRoundStart, 5.f);
}

void AToonTanksBase::HandleRoundStart()
{
	TArray<AActor*> AllPlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), AllPlayerControllers);

	for (auto PlayerController : AllPlayerControllers)
	{
		if (APlayerController* TankController = Cast<APlayerController>(PlayerController))
		{
			EnableMoveInput(TankController->GetPawn());
		}
	}

	GameState->MatchState = MatchState::RoundStarted;
	GetWorldTimerManager().SetTimer(RoundTimer, this, &AToonTanksBase::HandleRoundEnd, MaxRoundTime);
}

void AToonTanksBase::HandleRoundEnd()
{
	//End the round, add a 'winner' for the round
	GameState->TotalRoundsPlayed++;

	int TeamWithHighestScore = 0;

	for (auto Team : GameState->CurrentTeams)
	{
		if (Team.Score > GameState->CurrentTeams[TeamWithHighestScore].Score) TeamWithHighestScore = Team.TeamID;
	}

	//Set the round winner on the GameState
	GameState->WinningTeam = GameState->CurrentTeams[TeamWithHighestScore];

	GameState->CurrentTeams[TeamWithHighestScore].RoundsWon++;

	GameState->MatchState = MatchState::RoundEnded;
	GameState->MatchStateUpdated();

	TArray<AActor*> AllPlayerControllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), AllPlayerControllers);

	for (auto PlayerController : AllPlayerControllers)
	{
		if (APlayerController* TankController = Cast<APlayerController>(PlayerController))
		{
			bool bWasSuccessful = HandleTankDeath(TankController, true);

			if (bWasSuccessful)
			{
				if (APawn* TankPawn = TankController->GetPawn())
				{
					DisableMoveInput(TankPawn);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("NO PAWN TO DISABLE INPUT FOR"));
				}
			}
		}

	}

	//Set Timer for either Game End or new round

	if (GameState->TotalRoundsPlayed >= RoundsPerMatch)
	{
		//Trigger Match-End timer and function
		GetWorldTimerManager().SetTimer(RoundTimer, this, &AToonTanksBase::HandleMatchEnd, 10.f);
	}
	else
	{
		//Trigger New Round 
		GetWorldTimerManager().SetTimer(RoundTimer,this, &AToonTanksBase::HandlePreRound, 10.f);
	}

}

void AToonTanksBase::HandleMatchEnd()
{
	//Restart level
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


AActor* AToonTanksBase::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	//Cast the controller to a TTPlayerController
	ATTPlayerController* PlayerController = Cast<ATTPlayerController>(Player);
	//Loop through all player starts and pick a random one with the correct team ID. Team ID is in the player controller, but we'll have to cast it
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATTPlayerStart::StaticClass(), PlayerStarts);

		for (auto PlayerStart : PlayerStarts)
		{
			ATTPlayerStart* TTPlayerStart = Cast<ATTPlayerStart>(PlayerStart);

			if (IsValid(TTPlayerStart))
			{
				if (PlayerController->TeamID != -1)
				{
					// Check for a start which has the same TeamID, and hasn't aready been taken.
					if (TTPlayerStart->TeamID == PlayerController->TeamID && PlayerStart->Tags.Find(FName("Taken")) == INDEX_NONE)
					{
						//PlayerStart->Tags.Add(FName("Taken"));
						return PlayerStart;
					}
				}
				else
				{
					return PlayerStart;
				}
			
			}
		}


	return nullptr;
}

void AToonTanksBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ATTPlayerController* TankController = Cast<ATTPlayerController>(NewPlayer);
	if (TankController)
	{
		if (bStartPlayersAsSpectators)
		{
			if (!TankController->PlayerState->bIsSpectator)
			{
				RestartPlayer(NewPlayer);
			}
			else
			{
				TankController->PlayerState->bIsSpectator = true;
			}
		}
	}

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
}

void AToonTanksBase::PostLogin(APlayerController* NewPlayer)
{
	if (bStartPlayersAsSpectators)
	{
		NewPlayer->PlayerState->bIsSpectator = true;
	}

	Super::PostLogin(NewPlayer);
}

void AToonTanksBase::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	ATTPlayerController* TankController = Cast<ATTPlayerController>(NewPlayer);
	if (TankController)
	{
		TankController->bIsSpawned = true;
		TankController->GetPawn<ATankChar>()->SetTeamColour_Server(TankController->TeamID);
	}
}

int32 AToonTanksBase::GetNumPlayers()
{
	int32 PlayerCount = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerActor = Iterator->Get();
		if (PlayerActor && PlayerActor->PlayerState && PlayerActor->PlayerState->bIsSpectator == false)
		{
			PlayerCount++;
		}
	}
	return PlayerCount;
}


void AToonTanksBase::HandleGameOver(bool PlayerWon)
{
	//Show stats (Bullets fired, turrets destroyed)
	//If player died, show lose result + stats
	//UI Trigger event ^^^^^
}


void AToonTanksBase::ActorDied(AActor* DeadActor, AActor* Killer)
{
	ATTPlayerController* KillerController = Cast<ATTPlayerController>(Killer->GetInstigatorController<ATTPlayerController>());

	if (Cast<ATankChar>(DeadActor))
	{
		ATankChar* DeadActorPawn = Cast<ATankChar>(DeadActor);
		UE_LOG(LogTemp, Error, TEXT("Player has died"));

		//Add 1 to the score of the killing team
		if(KillerController) GameState->CurrentTeams[KillerController->TeamID].Score++;

		GameState->CurrentTeamsUpdated();

		//Respawn the player
		AController* PawnPlayerController = DeadActor->GetInstigatorController<AController>();
		if (PawnPlayerController)
		{
			///TODO: Check gamemode setup incase we're over the total score required
			HandleTankDeath(PawnPlayerController, true);

		}

		

		return;
	}

}

void AToonTanksBase::SpawnNewTank(AController* PlayerController)
{
	//RestartPlayer(PlayerController);
}

bool AToonTanksBase::RespawnTank(AController* PlayerController)
{
	ATTPlayerController* TankController = Cast<ATTPlayerController>(PlayerController);

	if (APawn* NewPawn = SpawnDefaultPawnFor(PlayerController, FindPlayerStart(PlayerController)))
	{
		PlayerController->Possess(NewPawn);
		TankController->bIsDead = false;
		TankController->bIsSpawned = true;
		TankController->OnPlayerSpawned.Broadcast();
		return true;
	}
	//return false;
	return true;

}

bool AToonTanksBase::HandleTankDeath(AController* PlayerController, bool bShouldRespawn)
{
	ATTPlayerController* TankController = Cast<ATTPlayerController>(PlayerController);

	if (APawn* OldPawn = TankController->GetPawn())
	{
		PlayerController->UnPossess();
		OldPawn->Destroy();
		TankController->bIsDead = true;
		TankController->bIsSpawned = false;
		TankController->OnPlayerDied.Broadcast();

		//Setup respawn timer, later we should base this on something other than a hard timer in the gamemode
		if (bShouldRespawn)
		{
			FTimerHandle RespawnTimer;
			FTimerDelegate RespawnTimerDelegate;
			RespawnTimerDelegate.BindUFunction(this, FName("RespawnTank"), PlayerController);
			GetWorldTimerManager().SetTimer(RespawnTimer, RespawnTimerDelegate, GameState->TimeToRespawn, false);
		}
	
	}
	return true;
}


void AToonTanksBase::DisableMoveInput(APawn* TankPawn)
{
	if (UCharacterMovementComponent* TankMoveComp = TankPawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		TankMoveComp->SetMovementMode(MOVE_None);
	}
}

void AToonTanksBase::EnableMoveInput(APawn* TankPawn)
{
	if (UCharacterMovementComponent* TankMoveComp = TankPawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		TankMoveComp->SetMovementMode(MOVE_Walking);
	}
}

void AToonTanksBase::PlayerHit()
{
	HitCount++;
	UE_LOG(LogTemp, Error, TEXT("Current hit count: %d"), HitCount);
}
