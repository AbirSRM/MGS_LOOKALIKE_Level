#include "BTTask_GetNextPatrolPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../EnemyAIController.h" // FIXED: Reaches up one directory to find your controller
#include "PatrolPath.h"
#include "Components/SplineComponent.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint()
{
    NodeName = TEXT("Get Next Patrol Point");
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // 1. Get our specific AI Controller
    AEnemyAIController* AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // 2. Get the assigned path. If no path is assigned, fail gracefully so the game doesn't crash.
    APatrolPath* PatrolPath = AIController->AssignedPatrolPath;
    if (!PatrolPath || !PatrolPath->PathSpline)
    {
        return EBTNodeResult::Failed;
    }

    USplineComponent* Spline = PatrolPath->PathSpline;
    int32 CurrentIndex = AIController->CurrentPatrolIndex;

    // 3. Get the exact 3D world location of the current point on the curve
    FVector TargetLocation = Spline->GetLocationAtSplinePoint(CurrentIndex, ESplineCoordinateSpace::World);

    // 4. Save that location to the Blackboard so the MoveTo node can use it
    OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, TargetLocation);

    // 5. Increment the index for the next time this task runs
    int32 NextIndex = CurrentIndex + 1;

    // If we reached the end of the line, loop back to the beginning (Point 0)
    if (NextIndex >= Spline->GetNumberOfSplinePoints())
    {
        NextIndex = 0;
    }

    AIController->CurrentPatrolIndex = NextIndex;

    return EBTNodeResult::Succeeded;
}