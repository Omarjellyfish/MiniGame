#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Resume) Btn_Resume->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	if (Btn_MainMenu) Btn_MainMenu->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
}

void UPauseMenuWidget::OnResumeClicked()
{
	// Close the menu
	RemoveFromParent();

	// Unpause and hide mouse
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
}

void UPauseMenuWidget::OnMainMenuClicked()
{
	// Ensure your main menu level is saved as "MainMenuMap"
	UGameplayStatics::SetGamePaused(GetWorld(), false); // Unpause before leaving!
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
}