#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API UBTTask_GetNextPatrolPoint : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GetNextPatrolPoint();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // Expose a dropdown to select which Blackboard Key we save the location to
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector PatrolLocationKey;
};