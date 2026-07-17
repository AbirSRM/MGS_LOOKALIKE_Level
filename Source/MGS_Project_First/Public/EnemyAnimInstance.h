// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Direction;

	// NEW: Pitch for Aim Offsets (Looking up and down)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Locomotion", meta = (AllowPrivateAccess = "true"))
	float Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
	class APawn* OwningPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
	class UPawnMovementComponent* MovementComponent;
};