#include "PlayerHud.h"
#include "Components/ProgressBar.h"

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