#include "EnemyBaseCharacter.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"       
#include "BrainComponent.h"     
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h" 

AEnemyBaseCharacter::AEnemyBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    MaxHealth = 100.0f;
    Health = MaxHealth;
    bIsDead = false;

    // NEW: Reduced patrol speed to half (Default is 600, so 300)
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AEnemyBaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

void AEnemyBaseCharacter::Tick(float DeltaTime) { Super::Tick(DeltaTime); }
void AEnemyBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) { Super::SetupPlayerInputComponent(PlayerInputComponent); }

float AEnemyBaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
        FName HitBoneName = PointDamageEvent->HitInfo.BoneName;

        if (HitBoneName == "head" || HitBoneName == "neck_01")
        {
            ActualDamage = MaxHealth;
        }
        else if (HitBoneName.ToString().Contains("arm") || HitBoneName.ToString().Contains("hand") || HitBoneName.ToString().Contains("leg") || HitBoneName.ToString().Contains("foot"))
        {
            ActualDamage *= 0.5f;
        }
    }

    Health -= ActualDamage;

    if (DamageTakenSound && Health > 0.0f)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetActorLocation());
    }
    else if (DeathSound && Health <= 0.0f)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (Health <= 0.0f)
    {
        Health = 0.0f;
        bIsDead = true;

        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            if (UBrainComponent* BrainComp = AIController->GetBrainComponent()) BrainComp->StopLogic("Enemy is dead");
        }

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCharacterMovement()->DisableMovement();
        GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
        GetMesh()->SetSimulatePhysics(true);
    }
    else
    {
        if (AnimInstance && HitReactMontage)
        {
            AnimInstance->Montage_Play(HitReactMontage);
        }
    }

    return ActualDamage;
}