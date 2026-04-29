#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
APlayerCharacter::APlayerCharacter()
{
	// 1. Setup the Camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera near the head
	FirstPersonCamera->bUsePawnControlRotation = true; // Let the mouse rotate the camera

	// 2. Setup the First Person Mesh (Arms)
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FirstPersonMesh->SetOnlyOwnerSee(true); // Only the player sees these arms
	FirstPersonMesh->SetupAttachment(FirstPersonCamera); // Attach arms to the camera
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	FirstPersonMesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// 3. Attack Setup
	AttackDamage = 35.0f; // 3 hits to kill a 100 HP enemy
	AttackRange = 200.0f; // Slightly longer than enemy range so you can space them
	AttackRadius = 45.0f; // A 45-unit thick sphere is perfect for melee
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add the Input Mapping Context to the player
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
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

void APlayerCharacter::Attack()
{
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
	DrawDebugSphere(GetWorld(), StartLocation, AttackRadius, 12, FColor::Blue, false, 2.0f);
	// Draw the ending sphere (Green if hit, Red if miss)
	DrawDebugSphere(GetWorld(), EndLocation, AttackRadius, 12, bHit ? FColor::Green : FColor::Red, false, 2.0f);
	// Draw a line connecting them
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);

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