#include "HealthPackSpawner.h"
#include "HealthPack.h"
#include "TimerManager.h" // We need this to use Unreal's timer system!

AHealthPackSpawner::AHealthPackSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnInterval = 5.0f;
}

void AHealthPackSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AHealthPackSpawner::SpawnHealthPack, SpawnInterval, true);
}

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