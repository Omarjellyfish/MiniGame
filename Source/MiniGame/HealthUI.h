#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthUI.generated.h"

// Forward declare the Progress Bar
class UProgressBar;

UCLASS()
class MINIGAME_API UHealthUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// meta = (BindWidget) forces the Blueprint to have a Progress Bar with this EXACT name
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	// The function we will call every time a character takes damage or heals
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHealthPercent(float CurrentHealth, float MaxHealth);
};