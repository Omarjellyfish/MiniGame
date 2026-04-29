#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

class UProgressBar; // Forward declaration

UCLASS()
class MINIGAME_API UPlayerHud : public UUserWidget
{
	GENERATED_BODY()

public:
	// The Player's Health Bar
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	// The Player's Attack Cooldown Bar
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CooldownBar;

	// Functions to update the UI
	void UpdateHealth(float CurrentHealth, float MaxHealth);
	void UpdateCooldown(float CurrentCooldown, float MaxCooldown);
};