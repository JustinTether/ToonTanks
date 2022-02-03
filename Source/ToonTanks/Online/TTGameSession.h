// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "Online.h"
#include "HttpModule.h"
#include "TTGameSession.generated.h"

USTRUCT(BlueprintType)
struct FServerDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString SessionName;

	UPROPERTY(BlueprintReadOnly)
	int CurrentPlayers;

	UPROPERTY(BlueprintReadOnly)
	int MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	FString ServerIP;

	UPROPERTY(BlueprintReadOnly)
	FBlueprintSessionResult SearchResult;

};

UCLASS()
class TOONTANKS_API ATTGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServersFoundDelegate, TArray<FServerDetails>, ServerList);

	ATTGameSession(const FObjectInitializer& ObjectInitializer);
	
	bool HostNewSession(TSharedPtr<const FUniqueNetId> UserID, FName Session, bool bIsLAN, bool bIsPresence, int32 MaximumPlayers);

	virtual void OnCreateSessionComplete(FName Session, bool bWasSuccessful);

	void OnStartOnlineGameComplete(FName Session, bool bWasSuccessful);

	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresense); //Maybe add minimum players?

	void OnFindSessionsComplete(bool bWasSuccesfull);

	bool JoinSession(TSharedPtr<const FUniqueNetId> UserID, FName SessionName, FOnlineSessionSearchResult Session);

	void OnJoinSessionComplete(FName Session, EOnJoinSessionCompleteResult::Type Result);

	void DestroySession(FName Session);

	virtual void OnDestroySessionComplete(FName Session, bool bWasSuccessful);

	void OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/* Calls the Session instance to find servers. GETS from our python server browser */

	/* Leaving session as Client, handles calling Destroy() on the actor and travelling you to main menu */
	void LeaveSession(APlayerController* Player);

	DECLARE_EVENT_OneParam(TTGameSession, FOnFindSessionsComplete, bool /*bWasSuccessful*/);
	FOnFindSessionsComplete FindSessionsCompleteEvent;

	DECLARE_EVENT_OneParam(TTGameSession, FOnFindServersFinished, TArray<FServerDetails>);
	FOnFindServersFinished OnFindServersFinishedEvent;

	DECLARE_EVENT_OneParam(TTGameSession, FOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type /*Result*/);
	FOnJoinSessionComplete JoinSessionCompleteEvent;

	FOnFindSessionsComplete& OnFindSessionsComplete() { return FindSessionsCompleteEvent; }
	FOnJoinSessionComplete& OnJoinSessionComplete() { return JoinSessionCompleteEvent; }
	FOnFindServersFinished& OnHttpRequestComplete() { return OnFindServersFinishedEvent;}

	FOnServersFoundDelegate OnServersFound;


public:
	TSharedPtr<class FOnlineSessionSearch> SearchResults;
	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

};
