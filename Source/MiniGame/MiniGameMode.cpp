#include "MiniGameMode.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h" // We will use Target Points in the level as spawn locations
#include "EndScreenWidget.h"

AMiniGameMode::AMiniGameMode()
{
	CurrentWave = 0;
	TotalWaves = 2; // Requirement: Design at least 2 waves
	EnemiesRemaining = 0;
	EnemyCountMultiplierPerWave = 2;
}

void AMiniGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Start the first wave!
	StartWave();
}

void AMiniGameMode::StartWave()
{
	CurrentWave++;

	if (CurrentWave > TotalWaves)
	{
		EndGame(true); // We beat all the waves!
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Starting Wave %d"), CurrentWave);

	// Formula for enemies per wave: Wave 1 = 3 enemies, Wave 2 = 6 enemies, etc.
	int32 EnemiesToSpawn = CurrentWave * EnemyCountMultiplierPerWave;
	SpawnEnemyWave(EnemiesToSpawn);
}

void AMiniGameMode::SpawnEnemyWave(int32 NumberOfEnemies)
{
	if (!EnemyClassToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyClassToSpawn is not set in the GameMode blueprint!"));
		return;
	}

	// Find all TargetPoints in the level to use as spawn locations
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), SpawnPoints);

	if (SpawnPoints.Num() > 0)
	{
		for (int i = 0; i < NumberOfEnemies; i++)
		{
			// Pick a random spawn point
			int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
			AActor* ChosenSpawnPoint = SpawnPoints[RandomIndex];

			// Spawn the enemy
			GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClassToSpawn, ChosenSpawnPoint->GetActorLocation(), ChosenSpawnPoint->GetActorRotation());

			EnemiesRemaining++;
		}
	}
}

void AMiniGameMode::OnEnemyDied()
{
	EnemiesRemaining--;
	UE_LOG(LogTemp, Warning, TEXT("Enemy died. Remaining: %d"), EnemiesRemaining);

	// Requirement: Next wave can't spawn until all enemies of previous wave are defeated
	if (EnemiesRemaining <= 0)
	{
		// Wait 3 seconds, then start the next wave (Bonus points for staggering/delays!)
		FTimerHandle NextWaveTimer;
		GetWorld()->GetTimerManager().SetTimer(NextWaveTimer, this, &AMiniGameMode::StartWave, 3.0f, false);
	}
}
void AMiniGameMode::OnPlayerDied()
{
	UE_LOG(LogTemp, Error, TEXT("The Player has died! Ending the game..."));

	// Trigger the End Screen with bIsWin = false
	EndGame(false);
}
void AMiniGameMode::EndGame(bool bIsWin)
{
	// 1. Print to the log so we can debug
	if (bIsWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("YOU WIN!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("YOU LOSE!"));
	}

	// 2. The safety trap: Did we forget to assign the widget in the editor?
	if (!EndScreenClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CRITICAL ERROR: EndScreenClass is NULL! Please assign WBP_EndScreen in BP_MiniGameMode."));
		return;
	}

	// 3. Create and show the UI for BOTH winning and losing
	UEndScreenWidget* EndWidget = CreateWidget<UEndScreenWidget>(GetWorld(), EndScreenClass);
	if (EndWidget)
	{
		EndWidget->AddToViewport();

		// This one line handles the Red/Green text swap!
		EndWidget->SetupScreen(bIsWin);

		// Freeze game and unlock mouse
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeUIOnly());
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

