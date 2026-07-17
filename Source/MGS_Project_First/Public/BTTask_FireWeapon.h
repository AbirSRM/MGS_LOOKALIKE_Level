#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FireWeapon.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API UBTTask_FireWeapon : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_FireWeapon();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};