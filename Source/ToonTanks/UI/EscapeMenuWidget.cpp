
#include "EscapeMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ToonTanks/Instances/TTGameInstance.h"


//UE4 Includes
#include "Components/Button.h"

void UEscapeMenuWidget::NativeConstruct()
{
	ExitGameButton->OnClicked.AddDynamic(this, &UEscapeMenuWidget::DestroyServer);
}

void UEscapeMenuWidget::DestroyServer()
{
	UTTGameInstance* TTInstance;

	TTInstance = Cast<UTTGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!IsValid(TTInstance))
	{
		return;
	}

	TTInstance->DestroySession();

	UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"), true);
}