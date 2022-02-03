#pragma once

//UE4 Includes
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"


//TT Includes
#include "EscapeMenuWidget.generated.h"

class UButton;

UCLASS()
class TOONTANKS_API UEscapeMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* ExitGameButton;

private:

	UFUNCTION()
	void DestroyServer();

protected:
	void NativeConstruct() override;
};