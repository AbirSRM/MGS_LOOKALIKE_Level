#include "PatrolPath.h"
#include "Components/SplineComponent.h"

APatrolPath::APatrolPath()
{
    // Paths don't need to tick, saving performance
    PrimaryActorTick.bCanEverTick = false;

    // Create the spline and make it the root of the actor
    PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
    RootComponent = PathSpline;
}