
//TTIncludes
#include "MainMenu.h"
#include "HostServerSettings.h"
#include "ServerBrowser.h"
#include "ServerBrowserResult.h"


//UE Includes
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainMenu::NativeConstruct()
{
	JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinButtonClicked);
	QuitGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitButtonClicked);
	HostGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostButtonClicked);
}

void UMainMenu::OnJoinButtonClicked()
{
	OnServerBrowserRequested.Broadcast();

}

void UMainMenu::OnHostButtonClicked()
{
	OnHostSettingsRequested.Broadcast();
}

void UMainMenu::OnQuitButtonClicked()
{
	GetWorld()->Exec(GetWorld(), TEXT("Exit"));
}