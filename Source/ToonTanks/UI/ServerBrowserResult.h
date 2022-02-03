

//UE4 Includes
#include "Blueprint/UserWidget.h"

//TT Includes
#include "ToonTanks/Online/TTGameSession.h"
#include "ServerBrowserResult.generated.h"

//Forward declarations
class UButton;
class UTextBlock;

DECLARE_LOG_CATEGORY_EXTERN(TTLogUI, Log, Log);

UCLASS()
class UServerBrowserResult : public UUserWidget
{
	GENERATED_BODY()

public:
	FServerDetails ServerInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentPlayers;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxPlayers;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Ping;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* JoinServerButton;

	void JoinServer();

protected:
void NativeConstruct() override;

};