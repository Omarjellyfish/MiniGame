#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Engine/World.h" 
#include "Animation/AnimInstance.h" // Required to check if Montages are playing
#include "Components/CapsuleComponent.h" // Required to set collision responses
#include "BaseCharacter.h"
AEnemyCharacter::AEnemyCharacter()
{
	// 1. ENABLE TICK (Crucial for our new chase logic)
	PrimaryActorTick.bCanEverTick = true;

	// Automatically assign Unreal's default AI Controller
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Keep the enemy facing the direction it is walking
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	// 2. Initialize Combat Stats
	MeleeRange = 150.0f; // How close they need to be to hit you.
	StoppingDistance = 90.0f; // <--- ADD THIS (90 is much closer than 130!)
	AttackDamage = 5.0f; // How much health you lose
	AttackCooldown = 1.5f; // Wait 1.5 seconds between attacks

	// Start this as a negative number so they can attack instantly on the first hit
	LastAttackTime = -AttackCooldown;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}
float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 1. Subtract damage and clamp to prevent negative health
	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Max(CurrentHealth, 0.0f);

	// 2. Call Super::TakeDamage SECOND. 
	// This triggers your Blueprint 'Event AnyDamage'. Because we updated Health first, 
	// the Blueprint will read the correct value (even if it's exactly 0).
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 3. Handle Death LAST
	if (CurrentHealth <= 0.0f)
	{
		Die();
	}

	return ActualDamage;
}
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Find the Player
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		// Calculate the distance between the Enemy and the Player
		float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		AAIController* AIController = Cast<AAIController>(GetController());

		// 3. Animation Check: Is the enemy currently attacking?
		bool bIsAttacking = false;
		if (AttackMontage && GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->Montage_IsPlaying(AttackMontage))
		{
			bIsAttacking = true;
		}

		// 4. Combat Logic
		if (DistanceToPlayer <= MeleeRange)
		{
			// Force the enemy to stop walking so they plant their feet to attack
			if (AIController)
			{
				AIController->StopMovement();
			}


			//  Get the direction from the enemy to the player
			FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - GetActorLocation();

			FRotator LookAtRotation = DirectionToPlayer.Rotation();

			LookAtRotation.Pitch = 0.0f;
			LookAtRotation.Roll = 0.0f;

			//  Apply the rotation
			SetActorRotation(LookAtRotation);
			// ---------------------------------------------------------

			// Cooldown Check
			if (GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown)
			{
				// Apply Damage to the player
				UGameplayStatics::ApplyDamage(PlayerPawn, AttackDamage, GetController(), this, UDamageType::StaticClass());

				if (AttackMontage)
				{
					PlayAnimMontage(AttackMontage);
				}

				// Reset the cooldown timer
				LastAttackTime = GetWorld()->GetTimeSeconds();
			}
		}
		else if (!bIsAttacking)
		{
			if (AIController)
			{
				// We subtract 20 from MeleeRange so they stop slightly before passing through you
				AIController->MoveToActor(PlayerPawn, StoppingDistance);
			}
		}
	}
}
void AEnemyCharacter::Die()
{
	
	Super::Die();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();

	if (Controller)
	{
		Controller->UnPossess();
	}

	SetLifeSpan(1.0f);
}