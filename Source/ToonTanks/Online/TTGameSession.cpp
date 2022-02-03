// Fill out your copyright notice in the Description page of Project Settings.


#include "TTGameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Kismet/GameplayStatics.h"
#include "SocketSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "../Controllers/TTPlayerController.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Online.h"
#include "../Instances/TTGameInstance.h"


ATTGameSession::ATTGameSession(const FObjectInitializer& ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ATTGameSession::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &ATTGameSession::OnStartOnlineGameComplete);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ATTGameSession::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ATTGameSession::OnJoinSessionComplete);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &ATTGameSession::OnDestroySessionComplete);
}

bool ATTGameSession::HostNewSession(TSharedPtr<const FUniqueNetId> UserID, FName Session, bool bIsLAN, bool bIsPresence, int32 MaximumPlayers)
{
	IOnlineSubsystem* NetSubsystem = IOnlineSubsystem::Get();

	if (NetSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetSubsystem->GetSessionInterface();

		if (SessionManager.IsValid() && UserID)
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings);
			//Match the session settings
			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->NumPublicConnections = MaximumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bUsesPresence = bIsPresence;

			FOnlineSessionSetting SessionGivenName;
			SessionGivenName.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineService;
			SessionGivenName.Data = Session.ToString();
			SessionSettings->Settings.Add(FName("SESSION_NAME"), SessionGivenName);

			SessionSettings->Set(SETTING_MAPNAME, FString("NewMain"), EOnlineDataAdvertisementType::ViaOnlineService);

			OnCreateSessionCompleteDelegateHandle = SessionManager->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			UE_LOG(LogTemp, Warning, TEXT("Online: Created lobby with SessionID: %s"), *Session.ToString());
			return SessionManager->CreateSession(*UserID, Session, *SessionSettings);
		
		}
	}
	return false;
}

void ATTGameSession::OnCreateSessionComplete(FName Session, bool bWasSuccessful)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();
	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();
		if (SessionManager.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			SessionManager->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = SessionManager->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				SessionManager->StartSession(Session);
			}
		}
	}
}

void ATTGameSession::OnStartOnlineGameComplete(FName Session, bool bWasSuccessful)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();
		if (SessionManager.IsValid())
		{
			SessionManager->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), "NewMain", true, "listen");

	}
}


void ATTGameSession::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresense)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();

		if (SessionManager.IsValid() && UserId->IsValid())
		{
			SearchResults = MakeShareable(new FOnlineSessionSearch()); //Ask someone about this cause I genuinely don't understand it

			SearchResults->bIsLanQuery = bIsLan;
			SearchResults->MaxSearchResults = 20;
			SearchResults->PingBucketSize = 120;

			if (bIsPresense)
			{
				SearchResults->QuerySettings.Set(SEARCH_PRESENCE, bIsPresense, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchResultsRef = SearchResults.ToSharedRef();
			OnFindSessionsCompleteDelegateHandle = SessionManager->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			SessionManager->FindSessions(*UserId, SearchResultsRef);
		}
	}
	else
	{
		OnFindSessionsComplete(false);
	}
}

void ATTGameSession::OnFindSessionsComplete(bool bWasSuccesfull)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionMananger = NetworkSubsystem->GetSessionInterface();

		if (SessionMananger.IsValid())
		{
			SessionMananger->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			if (SearchResults->SearchResults.Num() > 0)
			{
				UTTGameInstance* GameInstance = GetGameInstance<UTTGameInstance>();
				GameInstance->SetSearchResults(*SearchResults);
			}
			OnFindSessionsComplete().Broadcast(bWasSuccesfull);
		}
	}
}

bool ATTGameSession::JoinSession(TSharedPtr<const FUniqueNetId> UserID, FName ServerName, FOnlineSessionSearchResult Session)
{
	bool bWasSuccessful = false;

	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionMananger = NetworkSubsystem->GetSessionInterface();

		if (SessionMananger.IsValid() && UserID->IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = SessionMananger->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			FString SessionGivenName = Session.Session.SessionSettings.Settings.FindRef("SESSION_NAME").Data.ToString();

			ServerName = FName(*SessionGivenName);
			bWasSuccessful = SessionMananger->JoinSession(*UserID, ServerName, Session);
		}
	}

	return bWasSuccessful;
}

void ATTGameSession::OnJoinSessionComplete(FName Session, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();

		if (SessionManager.IsValid())
		{
			SessionManager->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			FString TravelURL;

			if (PlayerController && SessionManager->GetResolvedConnectString(Session, TravelURL))
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

/*Servers version of leave session. 
@PARAM Session -> FName of the session to be destroyed. */
void ATTGameSession::DestroySession(FName Session)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();
	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();

		if (SessionManager.IsValid())
		{
			UE_LOG(LogOnline, Log, TEXT("Session is %s"), *Session.ToString());
			SessionManager->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
			SessionManager->DestroySession(Session);
		}
	}
}

/*Client version of leave session, calls a Server RPC to destroy the actor and travels to main menu */
void ATTGameSession::LeaveSession(APlayerController* Player)
{
	if (ATTPlayerController* TankController = Cast<ATTPlayerController>(Player))
	{
		TankController->HandleLeaveSession();
	}
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true);
}


void ATTGameSession::OnDestroySessionComplete(FName Session, bool bWasSuccessful)
{
	IOnlineSubsystem* NetworkSubsystem = IOnlineSubsystem::Get();

	if (NetworkSubsystem)
	{
		IOnlineSessionPtr SessionManager = NetworkSubsystem->GetSessionInterface();

		if (SessionManager.IsValid())
		{
			FName GameSession(NAME_GameSession);
			SessionManager->ClearOnDestroySessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true);
			}
		}
	}
}

void ATTGameSession::OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (Request->GetVerb() == "GET")
	{
		//Got server list
		TArray<FServerDetails> ServerList;

		FServerDetails Server;

		FString ResponseFromServer = Response->GetContentAsString();

		TSharedPtr<FJsonObject> ResponseObject = MakeShareable(new FJsonObject());
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseFromServer);

		if (FJsonSerializer::Deserialize(JsonReader, ResponseObject))
		{
			FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseFromServer, &Server, 0, 0);

			ServerList.Add(Server);
		}

		OnServersFound.Broadcast(ServerList);
	}
}