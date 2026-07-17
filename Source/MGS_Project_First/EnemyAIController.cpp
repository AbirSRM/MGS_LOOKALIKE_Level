#include "EnemyAIController.h"
#include "Engine/Engine.h" 
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h" 
#include "Perception/AIPerceptionSystem.h" 
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h" 
#include "PatrolPath.h" 
#include "WeaponBase.h" 
#include "GameFramework/Pawn.h"
#include "EnemyBaseCharacter.h" 
#include "Math/UnrealMathUtility.h"

AEnemyAIController::AEnemyAIController()
{
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2500.f;
    SightConfig->LoseSightRadius = 3000.f;
    SightConfig->PeripheralVisionAngleDegrees = 75.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 3000.f;
    HearingConfig->SetMaxAge(3.f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

    PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->ConfigureSense(*HearingConfig);
    PerceptionComponent->ConfigureSense(*DamageConfig);

    PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);

    CurrentPatrolType = EPatrolType::AreaPatrol;
    AssignedPatrolPath = nullptr;
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(CombatTimerHandle, this, &AEnemyAIController::CombatTick, 0.2f, true);
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset != nullptr)
    {
        RunBehaviorTree(BehaviorTreeAsset);

        if (UBlackboardComponent* BBComp = GetBlackboardComponent())
        {
            BBComp->SetValueAsEnum(FName("PatrolType"), static_cast<uint8>(CurrentPatrolType));
        }
    }
}

void AEnemyAIController::CombatTick()
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    AEnemyBaseCharacter* EnemyPawn = Cast<AEnemyBaseCharacter>(GetPawn());

    if (!BBComp || !EnemyPawn || EnemyPawn->GetIsDead()) return;

    AActor* Target = Cast<AActor>(BBComp->GetValueAsObject("TargetActor"));
    if (Target)
    {
        float DistanceToTarget = FVector::Dist(GetPawn()->GetActorLocation(), Target->GetActorLocation());

        if (DistanceToTarget <= 3000.0f)
        {
            if (LineOfSightTo(Target))
            {
                SetFocus(Target);

                FVector DirectionToTarget = (Target->GetActorLocation() - EnemyPawn->GetActorLocation()).GetSafeNormal();
                FVector ForwardVector = EnemyPawn->GetActorForwardVector();

                float DotProd = FVector::DotProduct(ForwardVector, DirectionToTarget);
                float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProd));

                if (AngleDegrees <= 20.0f)
                {
                    TArray<AActor*> AttachedActors;
                    GetPawn()->GetAttachedActors(AttachedActors);
                    for (AActor* Actor : AttachedActors)
                    {
                        if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
                        {
                            Weapon->Fire();
                        }
                    }
                }
            }
        }
    }
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
    UBlackboardComponent* BBComp = GetBlackboardComponent();
    if (!BBComp || !Actor) return;

    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (Actor != PlayerPawn) return;

    // Identify which sense was triggered
    TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);

    if (Stimulus.WasSuccessfullySensed())
    {
        // 1. If they SEE you or FEEL a bullet, they attack immediately
        if (SenseClass == UAISense_Sight::StaticClass() || SenseClass == UAISense_Damage::StaticClass())
        {
            BBComp->SetValueAsObject(FName("TargetActor"), Actor);
            BBComp->ClearValue(FName("InvestigateLocation"));
            SetFocus(Actor);
        }
        // 2. If they HEAR you, they investigate instead of shooting blindly
        else if (SenseClass == UAISense_Hearing::StaticClass())
        {
            // Only care about the noise if they aren't already actively fighting you
            if (BBComp->GetValueAsObject(FName("TargetActor")) == nullptr)
            {
                BBComp->SetValueAsVector(FName("InvestigateLocation"), Stimulus.StimulusLocation);
                SetFocalPoint(Stimulus.StimulusLocation);
            }
        }
    }
    else
    {
        // Only lose track of the player if they walked out of sight 
        if (SenseClass == UAISense_Sight::StaticClass())
        {
            BBComp->ClearValue(FName("TargetActor"));
            BBComp->SetValueAsVector(FName("InvestigateLocation"), Stimulus.StimulusLocation);
            ClearFocus(EAIFocusPriority::Gameplay);
        }
    }
}