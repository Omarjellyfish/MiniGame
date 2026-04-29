#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "Kismet/GameplayStatics.h"

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		// 1. Run the Behavior Tree
		RunBehaviorTree(BehaviorTreeAsset);

		// 2. Find the Player
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

		// 3. Save the Player into the Blackboard's "TargetActor" key
		if (PlayerPawn && GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), PlayerPawn);
		}
	}
}