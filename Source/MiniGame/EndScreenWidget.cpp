#include "EndScreenWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UEndScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Restart) Btn_Restart->OnClicked.AddDynamic(this, &UEndScreenWidget::OnRestartClicked);
	if (Btn_MainMenu) Btn_MainMenu->OnClicked.AddDynamic(this, &UEndScreenWidget::OnMainMenuClicked);
}

void UEndScreenWidget::SetupScreen(bool bWon)
{
	if (Txt_Result)
	{
		if (bWon)
		{
			Txt_Result->SetText(FText::FromString("VICTORY!"));
			Txt_Result->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			Txt_Result->SetText(FText::FromString("GAME OVER"));
			Txt_Result->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
	}
}

void UEndScreenWidget::OnRestartClicked()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName())); // Reloads current map
}

void UEndScreenWidget::OnMainMenuClicked()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
}