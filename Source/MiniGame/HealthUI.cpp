#include "HealthUI.h"
#include "Components/ProgressBar.h" // We need this to modify the bar!

void UHealthUI::UpdateHealthPercent(float CurrentHealth, float MaxHealth)
{
	// Ensure we don't divide by zero, and that the HealthBar actually exists
	if (HealthBar && MaxHealth > 0.0f)
	{
		// A Progress bar goes from 0.0 (empty) to 1.0 (full)
		float HealthPercentage = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(HealthPercentage);
	}
}