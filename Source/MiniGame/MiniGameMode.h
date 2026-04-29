// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiniGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MINIGAME_API AMiniGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMiniGameMode();

	// Called by enemies when they die
	void OnEnemyDied();

protected:
	virtual void BeginPlay() override;

	// --- Wave System Variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System")
	int32 CurrentWave;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave System")
	int32 TotalWaves;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave System")
	int32 EnemiesRemaining;

	// The class of the enemy we want to spawn (We will set this to BP_EnemyCharacter in the editor)
	UPROPERTY(EditAnywhere, Category = "Wave System")
	TSubclassOf<class AEnemyCharacter> EnemyClassToSpawn;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UEndScreenWidget> EndScreenClass;


	// --- Functions ---
	void StartWave();
	void SpawnEnemyWave(int32 NumberOfEnemies);
	void EndGame(bool bIsWin);
	
};
