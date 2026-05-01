#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MINIGAME_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Enables per-frame updates for chasing
	virtual void Tick(float DeltaTime) override;
	virtual void Die() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	// --- Combat Variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MeleeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float StoppingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown;

	float LastAttackTime;
};