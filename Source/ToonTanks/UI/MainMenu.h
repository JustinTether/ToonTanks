#pragma once

//UE Includes
#include "Blueprint/UserWidget.h"
//TTIncludes
#include "MainMenu.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnServerBrowserRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHostSettingsRequested);

UCLASS()
class UMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnServerBrowserRequested OnServerBrowserRequested;
	
	UPROPERTY(BlueprintAssignable)
	FOnHostSettingsRequested OnHostSettingsRequested;
private:
	UPROPERTY(meta = (BindWidget))
	UButton* HostGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameButton;

	UFUNCTION()
	void OnJoinButtonClicked();
	UFUNCTION()
	void OnHostButtonClicked();
	UFUNCTION()
	void OnQuitButtonClicked();
protected:
	void NativeConstruct() override;
};

