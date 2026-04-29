#include "HealthPack.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h" // We need this to cast to the player!

AHealthPack::AHealthPack()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. Setup the Collision Sphere (The Root)
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	RootComponent = CollisionSphere;
	CollisionSphere->InitSphereRadius(50.0f);
	CollisionSphere->SetCollisionProfileName(TEXT("Trigger")); // Allows the player to walk through it

	// 2. Setup the visible Mesh
	PackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PackMesh"));
	PackMesh->SetupAttachment(RootComponent);
	PackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // So the player doesn't bump into the physical mesh

	// Default heal amount
	HealAmount = 25.0f;
}

void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

	// Bind our custom Overlap function to Unreal's actual collision event
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlapBegin);
}

void AHealthPack::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		// Try to cast whatever touched us to the PlayerCharacter
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player)
		{
			Player->Heal(HealAmount);

			// Destroy the health pack so it can't be picked up twice
			Destroy();
		}
	}
}