// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGameInstance.h"
#include "UObject/CoreOnline.h"
#include "Online.h"
#include "../Online/TTGameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemSessionSettings.h"
#include "FindSessionsCallbackProxy.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetworkDelegates.h"
#include "Engine/World.h"
#include "../Modes/ToonTanksBase.h"
#include "ToonTanks/Controllers/TTPlayerController.h"

UTTGameInstance::UTTGameInstance()
{

}

bool UTTGameInstance::CreateNewSession(int32 UserID, FName Session, bool bIsLAN, bool bIsPresence, int32 MaximumPlayers)
{
	ATTGameSession* TTOnlineSession = GetGameSession();
	ULocalPlayer* Player = GetFirstGamePlayer();
	ServerGivenName = Session;

	if (TTOnlineSession)
	{
		bool bWasSuccesfull = TTOnlineSession->HostNewSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), Session, bIsLAN, bIsPresence, MaximumPlayers);
		return bWasSuccesfull;
	}
	
	return false;
}


bool UTTGameInstance::FindSessions(bool bIsLan, bool bIsPresense)
{
	ATTGameSession* TTOnlineSession = GetGameSession();
	ULocalPlayer* Player = GetFirstGamePlayer();
	FOnlineSessionSearch SearchResults;
	bool bWasSuccessful = false;

	if (TTOnlineSession)
	{
		//Clear delegate from the OnFindSessionsComplete event, just incase
		TTOnlineSession->OnFindSessionsComplete().RemoveAll(this);

		//Add a new delegate
		OnSearchSessionsCompleteDelegateHandle = TTOnlineSession->OnFindSessionsComplete().AddUObject(this, &UTTGameInstance::OnSearchSessionsComplete);
		
		//Initiate the find sessions
		TTOnlineSession->FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), bIsLan, bIsPresense);
		bWasSuccessful = true;
	}
	return bWasSuccessful;
}

void UTTGameInstance::SetSearchResults(FOnlineSessionSearch SearchResult)
{
	//SessionSearchResults = SearchResult;
	
}

void UTTGameInstance::OnSearchSessionsComplete(bool bWasSuccessful)
{
	ATTGameSession* TTOnlineSession = GetGameSession();
	TArray<FString> ServerNames;
	if (TTOnlineSession)
	{
		//Unbind the delegate as we no longer need it
		TTOnlineSession->OnFindSessionsComplete().Remove(OnSearchSessionsCompleteDelegateHandle);

		for (auto result : TTOnlineSession->SearchResults->SearchResults)
		{
			FServerDetails ServerDetails;
			ServerDetails.SessionName = result.Session.SessionSettings.Settings.FindRef("SESSION_NAME").ToString();
			ServerDetails.MaxPlayers = result.Session.SessionSettings.NumPublicConnections;
			ServerDetails.CurrentPlayers = ServerDetails.MaxPlayers - result.Session.NumOpenPrivateConnections;
			ServerDetails.SearchResult.OnlineResult = result;
			OnServerSearchEnded.Broadcast(bWasSuccessful, ServerDetails);
		}
		if (TTOnlineSession->SearchResults->SearchResults.Num() == 0)
		{
			FServerDetails ServerDetails;
			bWasSuccessful = false;
			OnServerSearchEnded.Broadcast(bWasSuccessful, ServerDetails);
		}
		
	}
	
}

bool UTTGameInstance::JoinSession(FName ServerName, FBlueprintSessionResult Session)
{
	ATTGameSession* TTOnlineSession = GetGameSession();
	ULocalPlayer* Player = GetFirstGamePlayer();
	if (TTOnlineSession)
	{
		OnJoinSessionCompleteDelegateHandle = TTOnlineSession->OnJoinSessionComplete().AddUObject(this, &UTTGameInstance::OnJoinSessionFinished);
		if (TTOnlineSession->JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), ServerName, Session.OnlineResult))
		{
			return true;
		}
	}
	return false;
}

void UTTGameInstance::OnJoinSessionFinished(EOnJoinSessionCompleteResult::Type Result)
{
	ATTGameSession* TTOnlineSession = GetGameSession();
	if (TTOnlineSession)
	{
		TTOnlineSession->OnJoinSessionComplete().Remove(OnJoinSessionCompleteDelegateHandle);
	}
}

void UTTGameInstance::DestroySession()
{
	ATTPlayerController* PC;
	PC = Cast<ATTPlayerController>(GetFirstLocalPlayerController());

	if (!IsValid(PC))
	{
		return;
	}

	//TODO Change HandleTankDeath to take a boolean for respawn enabled
	PC->HandleLeaveSession();

	ATTGameSession* TTOnlineSession = GetGameSession();
	if (TTOnlineSession)
	{
		TTOnlineSession->DestroySession(ServerGivenName);
	}
}

ATTGameSession* UTTGameInstance::GetGameSession()
{
	UWorld* World = GetWorld();

	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			return Cast<ATTGameSession>(GameMode->GameSession);
		}
	}
	return nullptr;
}

void UTTGameInstance::ClientLeaveSession(APlayerController* Player)
{
	if (ATTGameSession* TTOnlineSession = GetGameSession())
	{
		TTOnlineSession->LeaveSession(Player);
	}
}

void UTTGameInstance::OnFindServersFinished(TArray<FServerDetails> ServerList)
{
	if (ATTGameSession* TTOnlineSession = GetGameSession())
	{
		TTOnlineSession->OnHttpRequestComplete().RemoveAll(this);
		OnGetServersFinished.Broadcast(ServerList);
	}
}


