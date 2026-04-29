#include "MainMenuGameMode.h"
#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		UMainMenuWidget* Menu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuClass);
		if (Menu)
		{
			Menu->AddToViewport();

			// Unlock the mouse and set it to UI only
			if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}