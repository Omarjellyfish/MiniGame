#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerHud.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "PauseMenuWidget.h"
#include "DrawDebugHelpers.h"
#include "MiniGameMode.h"
#include "PlayerHud.h" // (Make sure your HUD include is here too)


APlayerCharacter::APlayerCharacter()
{
	// 1. Setup the Camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	// ATTACH TO MESH SOCKET: This is the "correct" way for immersion.
	// Replace "CameraSocket" with the actual name of the socket on your skeleton (e.g., "head")
	FirstPersonCamera->SetupAttachment(GetMesh(), FName("CameraSocket"));

	// Reset relative transforms so it sits exactly ON the socket
	FirstPersonCamera->SetRelativeLocation(FVector::ZeroVector);
	FirstPersonCamera->SetRelativeRotation(FRotator::ZeroRotator);

	FirstPersonCamera->bUsePawnControlRotation = true;

	// 2. Setup the First Person Mesh (Arms)
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(FirstPersonCamera); // Arms follow the camera
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;

	// These offsets depend on your specific arm mesh assets
	FirstPersonMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FirstPersonMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// 3. Stats & Logic
	AttackDamage = 35.0f;
	AttackRange = 200.0f;
	AttackRadius = 45.0f;
	AttackCooldown = 1.5f;
	bCanAttack = true;

	PrimaryActorTick.bCanEverTick = true;
}
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}/*
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}*/

	// Add the Input Mapping Context to the player
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	if (PlayerHudClass)
	{
		PlayerHudInstance = CreateWidget<UPlayerHud>(GetWorld(), PlayerHudClass);
		if (PlayerHudInstance)
		{
			PlayerHudInstance->AddToViewport();
			PlayerHudInstance->UpdateHealth(CurrentHealth, MaxHealth); // Set initial health
			if (AMiniGameMode* GameMode = Cast<AMiniGameMode>(GetWorld()->GetAuthGameMode()))
			{
				PlayerHudInstance->UpdateWaveText(GameMode->CurrentWave);
			}
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the Actions to our Functions
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping (Using Unreal's built-in Jump functions from ACharacter)
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::Attack);

		// Pausing
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerCharacter::PauseGame);
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// Value is a Vector2D (W/S = Y axis, A/D = X axis)
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Move forward/backward
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		// Move right/left
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}


void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// Value is a Vector2D (Mouse X/Y)
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Look up/down and left/right
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void APlayerCharacter::Heal(float HealAmount)
{
	if (CurrentHealth > 0.0f)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

		// --- ADD THIS TO UPDATE THE HUD WHEN HEALED ---
		if (PlayerHudInstance)
		{
			PlayerHudInstance->UpdateHealth(CurrentHealth, MaxHealth);
		}
	}
}
void APlayerCharacter::Attack()
{
	if (!bCanAttack) return;
	// 2. Put the weapon on cooldown
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlayerCharacter::ResetAttack, AttackCooldown, false);
	// 1. Play the Animation
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}

	// 2. Setup the Trace Math
	FVector StartLocation = FirstPersonCamera->GetComponentLocation();
	FVector ForwardVector = FirstPersonCamera->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * AttackRange);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Don't stab ourselves

	// 3. Create the Sphere Shape
	FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);

	// 4. Fire the Sphere Sweep
	// We use SweepSingleByChannel instead of LineTraceSingleByChannel
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, Sphere, CollisionParams);

	// --- DEBUG VISUALS ---
	// Draw the starting sphere (Blue)
	//DrawDebugSphere(GetWorld(), StartLocation, AttackRadius, 12, FColor::Blue, false, 2.0f);
	//// Draw the ending sphere (Green if hit, Red if miss)
	//DrawDebugSphere(GetWorld(), EndLocation, AttackRadius, 12, bHit ? FColor::Green : FColor::Red, false, 2.0f);
	//// Draw a line connecting them
	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);

	// 5. Apply Damage
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(HitActor);
			if (HitCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("Sphere hit! Dealing %f damage to %s"), AttackDamage, *HitCharacter->GetName());
				UGameplayStatics::ApplyDamage(HitCharacter, AttackDamage, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

void APlayerCharacter::ResetAttack()
{
	bCanAttack = true;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smoothly update the cooldown bar if the HUD exists
	if (PlayerHudInstance)
	{
		if (!bCanAttack)
		{
			// Get exactly how much time has passed since we swung the knife
			float TimeElapsed = GetWorldTimerManager().GetTimerElapsed(AttackTimerHandle);
			PlayerHudInstance->UpdateCooldown(TimeElapsed, AttackCooldown);
		}
		else
		{
			// If we can attack, the bar is completely full
			PlayerHudInstance->UpdateCooldown(1.0f, 1.0f);
		}
	}
}
float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (PlayerHudInstance)
	{
		PlayerHudInstance->UpdateHealth(CurrentHealth, MaxHealth);
	}

	return ActualDamage;
}

void APlayerCharacter::PauseGame()
{
	
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if (ActivePauseMenu)
		{
			ActivePauseMenu->RemoveFromParent();
			ActivePauseMenu = nullptr; 
		}

		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	else
	{
		if (PauseMenuClass)
		{
			ActivePauseMenu = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
			if (ActivePauseMenu)
			{
				ActivePauseMenu->AddToViewport();

				if (APlayerController* PC = Cast<APlayerController>(Controller))
				{
					PC->bShowMouseCursor = true;
					PC->SetInputMode(FInputModeGameAndUI());
				}
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}
		}
	}
}