#include "BaseCharacter.h"
#include "Animation/AnimMontage.h"     // Required to play Montages
#include "GameFramework/Controller.h"  // Required to unpossess the character
#include "Engine/DamageEvents.h"       // Required for the TakeDamage function
#include "MiniGameMode.h"             // Required to call OnEnemyDied when an enemy dies
#include "Components/CapsuleComponent.h" // Required for capsule component
#include "GameFramework/CharacterMovementComponent.h" // Required for character movement
// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	// Start alive
	bIsDead = false;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	printf("Max Health: %f", MaxHealth);
	// Ensure the character starts with full health
	CurrentHealth = MaxHealth;
}

// Unreal's built-in damage function
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 1. If we are already dead, ignore the damage!
	if (bIsDead) return 0.0f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		CurrentHealth -= ActualDamage;
		UE_LOG(LogTemp, Warning, TEXT("%s took damage! Current Health: %f"), *GetName(), CurrentHealth);

		if (CurrentHealth <= 0.0f)
		{
			Die();
		}
	}
	return ActualDamage;
}
// Our custom death logic
void ABaseCharacter::Die()
{
	// Lock the function so it can never run twice
	if (bIsDead) return;
	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// --- THE FIX IS HERE ---
	// 1. Turn off the capsule collision so the player can walk through the dead body
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2. Turn off the movement system so gravity doesn't pull them through the floor!
	GetCharacterMovement()->DisableMovement();
	// -----------------------

	// ONLY unpossess if it is an AI enemy. 
	if (Controller && !IsPlayerControlled())
	{
		Controller->UnPossess();
	}

	// Tell the GameMode exactly who died
	if (AMiniGameMode* GameMode = Cast<AMiniGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if (IsPlayerControlled())
		{
			GameMode->OnPlayerDied();
		}
		else
		{
			GameMode->OnEnemyDied();
		}
	}

	SetLifeSpan(3.0f);
}