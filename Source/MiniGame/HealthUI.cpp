#include "HealthUI.h"
#include "Components/ProgressBar.h" 

void UHealthUI::UpdateHealthPercent(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		float HealthPercentage = CurrentHealth / MaxHealth;
		HealthBar->SetPercent(HealthPercentage);
	}
}