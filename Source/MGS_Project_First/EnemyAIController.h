#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UBehaviorTree;
class APatrolPath;

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
    Sniper UMETA(DisplayName = "Sniper"),
    LinePatrol UMETA(DisplayName = "Line Patrol"),
    AreaPatrol UMETA(DisplayName = "Area Patrol")
};

UCLASS()
class MGS_PROJECT_FIRST_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
    EPatrolType CurrentPatrolType;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI | Patrol")
    APatrolPath* AssignedPatrolPath;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Patrol")
    int32 CurrentPatrolIndex = 0;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Sight* SightConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Hearing* HearingConfig;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Perception")
    UAISenseConfig_Damage* DamageConfig;

    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

private:
    FTimerHandle CombatTimerHandle;
    void CombatTick();
};