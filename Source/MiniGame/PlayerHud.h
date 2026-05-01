#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHud.generated.h"

class UProgressBar; 
class UTextBlock; 
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

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_WaveNumber; 

	// Functions to update the UI
	void UpdateHealth(float CurrentHealth, float MaxHealth);
	void UpdateCooldown(float CurrentCooldown, float MaxCooldown);
	void UpdateWaveText(int32 NewWave);
};