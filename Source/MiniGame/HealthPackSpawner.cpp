#include "HealthPackSpawner.h"
#include "HealthPack.h"
#include "TimerManager.h" // We need this to use Unreal's timer system!

AHealthPackSpawner::AHealthPackSpawner()
{
	// Turn off Tick! Timers are much better for performance than checking every single frame
	PrimaryActorTick.bCanEverTick = false;

	// Set default values here
	SpawnInterval = 5.0f;
}

void AHealthPackSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Start a timer that calls SpawnHealthPack() every 'SpawnInterval' seconds, and set it to loop (true)
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AHealthPackSpawner::SpawnHealthPack, SpawnInterval, true);
}

// Notice the AHealthPackSpawner:: prefix! This connects it to the class.
void AHealthPackSpawner::SpawnHealthPack()
{
	if (HealthPackClass)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		GetWorld()->SpawnActor<AHealthPack>(HealthPackClass, SpawnLocation, SpawnRotation);

		UE_LOG(LogTemp, Warning, TEXT("Health Pack Spawned!"));
	}
}