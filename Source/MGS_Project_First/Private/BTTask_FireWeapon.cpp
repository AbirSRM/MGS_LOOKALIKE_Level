#include "BTTask_FireWeapon.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"

UBTTask_FireWeapon::UBTTask_FireWeapon()
{
    NodeName = TEXT("Fire Weapon");
}

EBTNodeResult::Type UBTTask_FireWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ACharacter* EnemyChar = Cast<ACharacter>(AIController->GetPawn());
    if (!EnemyChar) return EBTNodeResult::Failed;

    TArray<AActor*> AttachedActors;

    // THE FIX: The 'true' at the end forces the engine to search the child components (the Mesh) for the attached weapon!
    EnemyChar->GetAttachedActors(AttachedActors, false, true);

    for (AActor* Actor : AttachedActors)
    {
        if (AWeaponBase* Weapon = Cast<AWeaponBase>(Actor))
        {
            Weapon->Fire();
            return EBTNodeResult::Succeeded;
        }
    }

    // Prints a warning to the Output Log if it still can't find the gun
    UE_LOG(LogTemp, Warning, TEXT("AI tried to fire, but could not find a WeaponBase attached to its mesh!"));
    return EBTNodeResult::Failed;
}