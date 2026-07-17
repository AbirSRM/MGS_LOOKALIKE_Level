#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API UPlayerAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    class APlayerCharacter* PlayerCharacter;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "Locomotion")
    bool bIsCrouching;

    // --- Combat Variables ---
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsFiring;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float AimPitch;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FVector AimTargetLocation;
};