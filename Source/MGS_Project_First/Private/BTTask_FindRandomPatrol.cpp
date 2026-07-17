#include "BTTask_FindRandomPatrol.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

UBTTask_FindRandomPatrol::UBTTask_FindRandomPatrol()
{
    NodeName = TEXT("Find Random Patrol Location");
    SearchRadius = 1500.0f; // Default radius fallback
}

EBTNodeResult::Type UBTTask_FindRandomPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // Get the AI Controller and its Pawn
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
    {
        return EBTNodeResult::Failed;
    }

    // Access the Navigation System
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys)
    {
        return EBTNodeResult::Failed;
    }

    FNavLocation RandomLocation;
    FVector Origin = AIPawn->GetActorLocation();

    // Find a random point on the NavMesh within the set radius
    if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation))
    {
        // Save the successful location to the Blackboard key selected in the editor
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);

        return EBTNodeResult::Succeeded;
    }

    // Fails if it cannot find a valid NavMesh point (e.g., radius is off the map)
    return EBTNodeResult::Failed;
}