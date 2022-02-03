#pragma once

//UE4 Includes
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

//TT Includes
#include "HostServerSettings.generated.h"

class UButton;
class UCheckBox;
class UEditableText;

DECLARE_LOG_CATEGORY_EXTERN(TTLogHostWidget, Log, All);

UCLASS()
class UHostServerSettings : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UButton* StartSessionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* IsLanCheckbox;

	UPROPERTY(meta = (BindWidget))
	UEditableText* ServerName;



private:
	/*Begin a Steam session with ?listen enabled */
	UFUNCTION()
	void StartSession();

	/*Return to the previous menu */
	void PreviousMenu();

	/*Travel to a specified level with the specified travel arguments */
	void TravelLevel(FString MapName, FString TravelArgs, bool bIsSeamless);

	bool bIsLan;

	FString ServerNameString;

protected:
	void NativeConstruct() override;

};