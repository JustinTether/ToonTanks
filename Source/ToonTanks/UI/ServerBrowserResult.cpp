
//TT Includes
#include "ServerBrowserResult.h"
#include "ToonTanks/Online/TTGameSession.h"
#include "ToonTanks/Instances/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"

//UE4 Includes
#include "Components/Button.h"
#include "Components/TextBlock.h"

DEFINE_LOG_CATEGORY(TTLogUI)

void UServerBrowserResult::NativeConstruct()
{
	JoinServerButton->OnClicked.AddDynamic(this, &UServerBrowserResult::JoinServer);
}

void UServerBrowserResult::JoinServer()
{
	UTTGameInstance* TTGameInstance;

	TTGameInstance = Cast<UTTGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!IsValid(TTGameInstance))
	{
		UE_LOG(TTLogUI, Error, TEXT("Unable to resolve Game Instance"));
		return;
	}

	TTGameInstance->JoinSession(FName(*ServerInfo.SessionName), ServerInfo.SearchResult);

	RemoveFromParent();
}

