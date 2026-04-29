#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPackSpawner.generated.h"

// Forward declaration
class AHealthPack;

UCLASS()
class MINIGAME_API AHealthPackSpawner : public AActor
{
	GENERATED_BODY()

public:
	AHealthPackSpawner();

protected:
	virtual void BeginPlay() override;

	// Variables go in the header!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AHealthPack> HealthPackClass;

	// The function declaration
	void SpawnHealthPack();

	// A timer to handle the 5-second repeating countdown
	FTimerHandle SpawnTimerHandle;
};