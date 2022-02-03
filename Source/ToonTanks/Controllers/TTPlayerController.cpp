// Fill out your copyright notice in the Description page of Project Settings.


#include "TTPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "../Modes/ToonTanksBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "../Pawns/TankChar.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TTPlayerState.h"

ATTPlayerController::ATTPlayerController()
{
	TeamID = -1;
}

void ATTPlayerController::BeginPlay()
{
	GameMode = Cast<AToonTanksBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ATTPlayerController::HandleLeaveSession_Implementation()
{
	if (ATankChar* PlayerPawn = GetPawn<ATankChar>())
	{
		GameMode->HandleTankDeath(PlayerPawn->Controller, false);
	}
}

void ATTPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Pawn possessd. Current info: Team: %i, Name: %s, Pawn name: %s"), this->TeamID, * this->GetName(), *this->GetPawn()->GetName());
	
	if (ATankChar* TankPawn = GetPawn<ATankChar>())
	{
		TankPawn->TankTeam = TeamID;
		TankPawn->SetTeamColour_Server(TankPawn->TankTeam);
	}
}



void ATTPlayerController::SpawnNewTank(ATTPlayerController* TankController, int Team)
{	
	SpawnNewTank_Server(TankController, Team);
	//OnPlayerSpawned.Broadcast();
}

void ATTPlayerController::SpawnNewTank_Server_Implementation(ATTPlayerController* TankController, int Team)
{
	if (GameMode)
	{
		TankController->TeamID = Team;
		UE_LOG(LogTemp, Warning, TEXT("server attempting to spawn player %s They have a TeamID of %i"), *TankController->GetName(), TankController->TeamID);

		APlayerController* TankPlayer = Cast<APlayerController>(TankController);
		if (!TankPlayer) return;

		TankController->PlayerState->bIsSpectator = 0;
		//GameMode->RestartPlayer(TankPlayer);
		GameMode->RespawnTank(TankPlayer);
		//TankPlayer->GetPawn<ATankChar>()->SetTeamColour_Server(TankController->TeamID); //TODO: Come back to this later, it's broken since using a char
	}
}

void ATTPlayerController::OnRep_TeamID()
{
	ATankChar* TankPawn = GetPawn<ATankChar>();

	if (!IsValid(TankPawn))
	{
		return;
	}

	ATTPlayerController* TankController = TankPawn->GetInstigatorController<ATTPlayerController>();
	if (IsValid(TankController))
	{
		int TankTeam = TankController->TeamID;
		TankPawn->SetTeamColour_Server(TankTeam);
	}
	


}

void ATTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATTPlayerController, TeamID);
	DOREPLIFETIME(ATTPlayerController, bIsSpawned)
	DOREPLIFETIME(ATTPlayerController, bIsDead)
	
}

void ATTPlayerController::OnRep_bIsSpawned()
{
	if (bIsSpawned)
	{
		ATankChar* TankPawn = GetPawn<ATankChar>();
		if (IsValid(TankPawn))
		{
			int TankTeam = TankPawn->GetInstigatorController<ATTPlayerController>()->TeamID;
			TankPawn->SetTeamColour_Server(TankTeam);
		}
		bIsDead = false;
		OnPlayerSpawned.Broadcast();
	}

}

void ATTPlayerController::OnRep_bIsDead()
{
	if (bIsDead)
	{
		OnPlayerDied.Broadcast();
	}
}




