#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindRandomPatrol.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API UBTTask_FindRandomPatrol : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FindRandomPatrol();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    // Expose a radius variable so we can change it in the Behavior Tree editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float SearchRadius;

    // Expose a dropdown to select which Blackboard Key we save the location to
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    struct FBlackboardKeySelector PatrolLocationKey;
};