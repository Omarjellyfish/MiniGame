#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h" // Required for Enhanced Input
#include "PlayerCharacter.generated.h"

// Forward declarations to keep compilation fast
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;

class UPlayerHud;
UCLASS()
class MINIGAME_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

	// --- COMPONENTS ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

	// --- ENHANCED INPUT ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;


	// -- ATTACK INPUT ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRadius;
	void Attack();

	// UI Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UPauseMenuWidget> PauseMenuClass;

	UPROPERTY()
	UPauseMenuWidget* ActivePauseMenu;

	void PauseGame();

	// Input Handler Functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	// --- UI SYSTEM ---
	// The HUD Blueprint we will design in the editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UPlayerHud> PlayerHudClass;

	// The actual HUD instance created on the screen
	UPROPERTY()
	UPlayerHud* PlayerHudInstance;

	// --- COOLDOWN SYSTEM ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown;

	bool bCanAttack;
	FTimerHandle AttackTimerHandle;


	void ResetAttack();
public:
	virtual void Tick(float DeltaTime) override;
	// Overriding the setup input function
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float HealAmount);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};