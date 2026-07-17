#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBaseCharacter.generated.h"

class UAnimMontage;
class USoundBase;

UCLASS()
class MGS_PROJECT_FIRST_API AEnemyBaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBaseCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // NEW: Added getter to check if the enemy is dead
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool GetIsDead() const { return bIsDead; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
    float MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float Health;

    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    bool bIsDead;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    UAnimMontage* HitReactMontage;

    // NEW ENEMY AUDIO SLOTS
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Audio")
    USoundBase* DamageTakenSound; // Guard grunt/grunt impact sound

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Audio")
    USoundBase* DeathSound; // Guard flatlining/screaming sound
};