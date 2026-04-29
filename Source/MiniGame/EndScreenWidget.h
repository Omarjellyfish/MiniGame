#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndScreenWidget.generated.h"

UCLASS()
class MINIGAME_API UEndScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// The function our GameMode will call
	void SetupScreen(bool bWon);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_Result;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_MainMenu;

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();
};