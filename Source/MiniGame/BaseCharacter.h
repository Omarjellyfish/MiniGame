#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class MINIGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// --- HEALTH SYSTEM ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	// --- ANIMATIONS ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	UAnimMontage* DeathMontage;

	// --- FUNCTIONS ---
	// A custom function we will call when health reaches zero
	virtual void Die();

public:
	// Overriding Unreal's built-in damage system
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};