#include "BaseCharacter.h"
#include "Animation/AnimMontage.h"     // Required to play Montages
#include "GameFramework/Controller.h"  // Required to unpossess the character
#include "Engine/DamageEvents.h"       // Required for the TakeDamage function
#include "MiniGameMode.h"             // Required to call OnEnemyDied when an enemy dies
// Sets default values
ABaseCharacter::ABaseCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize our health variables
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
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
	// Call the base class logic first
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		CurrentHealth -= ActualDamage;

		// Print to the Output Log so we know it works during testing
		UE_LOG(LogTemp, Warning, TEXT("%s took damage! Current Health: %f"), *GetName(), CurrentHealth);

		// Check if we should die
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
	UE_LOG(LogTemp, Warning, TEXT("%s has died!"), *GetName());

	// Play the death animation if we assigned one
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// Disable collision so the player doesn't walk into invisible dead bodies
	SetActorEnableCollision(false);

	// If an AI or Player is controlling this, detach them
	if (Controller)
	{
		Controller->UnPossess();
	}
	if (Controller)
	{
		Controller->UnPossess();
	}

	// Tell the GameMode an enemy died (make sure it's an enemy, not the player!)
	if (ActorHasTag("Enemy"))
	{
		if (AMiniGameMode* GameMode = Cast<AMiniGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->OnEnemyDied();
		}
	}
	SetLifeSpan(3.0f);
}