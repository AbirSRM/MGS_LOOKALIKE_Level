#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

class USplineComponent;

UCLASS()
class MGS_PROJECT_FIRST_API APatrolPath : public AActor
{
    GENERATED_BODY()

public:
    APatrolPath();

    // The visual spline component designers will manipulate in the editor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Path")
    USplineComponent* PathSpline;
};