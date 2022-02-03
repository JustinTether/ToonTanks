// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "../Online/TTGameSession.h"
#include "TTGameInstance.generated.h"

class OnlineSessionSettings;
class ATTGameSession;



UCLASS()
class TOONTANKS_API UTTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FServerSearchEndedDelegate, bool, bWasSuccessful, FServerDetails, ServerItem);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetServersFinished, TArray<FServerDetails>, ServerList);

public:
	FDelegateHandle OnSearchSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	UFUNCTION(BlueprintCallable)
	bool CreateNewSession(int32 UserID, FName Session, bool bIsLAN, bool bIsPresence, int32 MaximumPlayers);

	UFUNCTION(BlueprintCallable)
	bool FindSessions(bool bIsLan, bool bIsPresense);

	void SetSearchResults(FOnlineSessionSearch SearchResult);

	void OnSearchSessionsComplete(bool bWasSuccessful);

	UFUNCTION(BlueprintCallable)
	bool JoinSession(FName ServerName, FBlueprintSessionResult Session);

	void OnJoinSessionFinished(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	void OnFindServersFinished(TArray<FServerDetails> ServerList);

	UFUNCTION(BlueprintCallable)
	void ClientLeaveSession(APlayerController* Player);

	ATTGameSession* GetGameSession();

public:
	UTTGameInstance();

	UPROPERTY(BlueprintAssignable)
	FServerSearchEndedDelegate OnServerSearchEnded;
protected:

	FName ServerGivenName;

	UPROPERTY(BlueprintAssignable)
	FGetServersFinished OnGetServersFinished;


};
