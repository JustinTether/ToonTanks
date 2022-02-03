
//TTIncludes
#include "ServerBrowser.h"
#include "ServerBrowserResult.h"
#include "ToonTanks/Instances/TTGameInstance.h"

//UE4 Includes
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CircularThrobber.h"
#include "Kismet/GameplayStatics.h"

void UServerBrowser::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(ServerList))
	{
		UE_LOG(TTLogUI, Error, TEXT("Unable to resolve ServerList UScrollBox!"));
		return;
	}
	ServerList->ClearChildren();

	if (!IsValid(RefreshButton))
	{
		UE_LOG(TTLogUI, Error, TEXT("Unable to resolve RefreshButton UButton!"));
		return;
	}
	RefreshButton->OnClicked.AddDynamic(this, &UServerBrowser::RefreshServerList);

	if (!IsValid(NoServersFound))
	{
		UE_LOG(TTLogUI, Error, TEXT("Unable to resolve NoServersFound UTextBlock!"));
		return;
	}
	NoServersFound->SetVisibility(ESlateVisibility::Visible);
}


void UServerBrowser::RefreshServerList()
{
	LoadingThrobber->SetVisibility(ESlateVisibility::Visible);
	NoServersFound->SetVisibility(ESlateVisibility::Collapsed);

	UTTGameInstance* TTInstance;
	TTInstance = Cast<UTTGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!IsValid(TTInstance))
	{
		UE_LOG(TTLogUI, Error, TEXT("Could not resolve game instance"));
		return;
	}
	//Clear any binds from previous search attempts
	TTInstance->OnServerSearchEnded.RemoveAll(this);

	TTInstance->FindSessions(false, false);
	TTInstance->OnServerSearchEnded.AddDynamic(this, &UServerBrowser::OnServerFound);
}

void UServerBrowser::OnServerFound(bool bWasSuccessful, FServerDetails ServerInfo)
{
	LoadingThrobber->SetVisibility(ESlateVisibility::Collapsed);

	if (!bWasSuccessful)
	{
		NoServersFound->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UServerBrowserResult* SearchResult = Cast<UServerBrowserResult>(CreateWidget(GetOwningPlayer(), UServerBrowserResult::StaticClass()));

	if (!IsValid(SearchResult))
	{
		return;
	}
	
	SearchResult->ServerName->SetText(FText::FromString(ServerInfo.SessionName));
	SearchResult->CurrentPlayers->SetText(FText::AsNumber(ServerInfo.CurrentPlayers));
	SearchResult->MaxPlayers->SetText(FText::AsNumber(ServerInfo.MaxPlayers));
	SearchResult->Ping->SetText(FText::AsNumber(ServerInfo.SearchResult.OnlineResult.PingInMs));

	UScrollBoxSlot* NewSlot = Cast<UScrollBoxSlot>(ServerList->AddChild(SearchResult));
	NewSlot->SetPadding(FMargin(0.0, 50.0, 0.0, 0.0));
}
