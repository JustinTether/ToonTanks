//TT Includes
#include "HostServerSettings.h"
#include "ToonTanks/Instances/TTGameInstance.h"

//UE4 Includes
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/CheckBox.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(TTLogHostWidget);

void UHostServerSettings::NativeConstruct()
{
	StartSessionButton->OnClicked.AddDynamic(this, &UHostServerSettings::StartSession);
}

void UHostServerSettings::StartSession()
{
	bIsLan = IsLanCheckbox->IsChecked();

	if (bIsLan)
	{
		TravelLevel("NewMain", "?listen", true);
		return;
	}

	UTTGameInstance* TTGameInstance;

	TTGameInstance = Cast<UTTGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if(!IsValid(TTGameInstance))
	{
		UE_LOG(TTLogHostWidget, Error, TEXT("Could not resolve UTTGameInstance attempting to start session"));
		return;
	}
	
	//Grab the server name and save it, along with 
	ServerNameString = ServerName->GetText().ToString();
	TTGameInstance->CreateNewSession(0, FName(*ServerNameString), bIsLan, false, 100);

	TravelLevel("NewMain", "?listen", true);
}

void UHostServerSettings::TravelLevel(FString MapName, FString TravelArgs, bool bIsSeamless)
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*MapName), bIsSeamless, TravelArgs);
}

void UHostServerSettings::PreviousMenu()
{
	//TODO: Will NOT work until MainMenu is ported to C++
}