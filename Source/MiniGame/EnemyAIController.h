// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTree;
/**
 * 
 */
UCLASS()
class MINIGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// The Behavior Tree asset we will create in the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

protected:
	// Called when the Controller takes over the Enemy Character
	virtual void OnPossess(APawn* InPawn) override;
};
