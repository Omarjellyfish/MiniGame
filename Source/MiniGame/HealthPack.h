#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h" 
#include "HealthPack.generated.h"

class USphereComponent;
class UStaticMeshComponent;


UCLASS()
class MINIGAME_API AHealthPack : public AActor
{
	GENERATED_BODY()

public:
	AHealthPack();

protected:
	virtual void BeginPlay() override;

	// The physical look of the health pack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PackMesh;

	// The invisible sphere that detects the player stepping on it
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionSphere;

	// How much health it restores
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float HealAmount;

	// The function that fires when the player touches it
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};