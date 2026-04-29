#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Engine/World.h" 
#include "Animation/AnimInstance.h" // Required to check if Montages are playing
#include "Components/CapsuleComponent.h" // Required to set collision responses
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
	MeleeRange = 150.0f; // How close they need to be to hit you
	AttackDamage = 15.0f; // How much health you lose
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

			// Cooldown Check
			if (GetWorld()->GetTimeSeconds() - LastAttackTime >= AttackCooldown)
			{
				// Apply Damage to the player
				UGameplayStatics::ApplyDamage(PlayerPawn, AttackDamage, GetController(), this, UDamageType::StaticClass());

				// Play the attack animation
				if (AttackMontage)
				{
					PlayAnimMontage(AttackMontage);
				}

				// Reset the cooldown timer
				LastAttackTime = GetWorld()->GetTimeSeconds();
			}
		}
		// 5. Chase Logic: ONLY chase if we are NOT currently in an attack animation
		else if (!bIsAttacking)
		{
			if (AIController)
			{
				// We subtract 20 from MeleeRange so they stop slightly before passing through you
				AIController->MoveToActor(PlayerPawn, MeleeRange - 20.0f);
			}
		}
	}
}