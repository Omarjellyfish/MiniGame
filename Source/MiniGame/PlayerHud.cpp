#include "PlayerHud.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h" 
void UPlayerHud::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UPlayerHud::UpdateCooldown(float CurrentCooldown, float MaxCooldown)
{
	if (CooldownBar && MaxCooldown > 0.0f)
	{
		CooldownBar->SetPercent(CurrentCooldown / MaxCooldown);
	}
}

void UPlayerHud::UpdateWaveText(int32 NewWave)
{
	if (Txt_WaveNumber)
	{
		// Combine the word "WAVE: " with the integer variable
		FString WaveString = FString::Printf(TEXT("WAVE: %d"), NewWave);

		// Set the text on the screen
		Txt_WaveNumber->SetText(FText::FromString(WaveString));
	}
}