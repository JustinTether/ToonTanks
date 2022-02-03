
//UE4 Includes
#include "Blueprint/UserWidget.h"
//TTIncludes
#include "ToonTanks/Online/TTGameSession.h"
#include "ServerBrowser.generated.h"

class UScrollBox;
class UCircularThrobber;
class UTextBlock;
class UButton;

UCLASS()
class UServerBrowser : public UUserWidget
{
	GENERATED_BODY()

public:
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* RefreshButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerList = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoServersFound = nullptr;

	UPROPERTY(meta = (BindWidget))
	UCircularThrobber* LoadingThrobber = nullptr;

	UFUNCTION()
	void RefreshServerList();

	UFUNCTION()
	void OnServerFound(bool bWasSuccessful, FServerDetails ServerInfo);

protected:
	void NativeConstruct() override;
};