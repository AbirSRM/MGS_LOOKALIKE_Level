// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "KismetAnimationLibrary.h" 
#include "Kismet/KismetMathLibrary.h" // NEW: Required for Rotator math

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
	if (OwningPawn)
	{
		MovementComponent = OwningPawn->GetMovementComponent();
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningPawn == nullptr)
	{
		OwningPawn = TryGetPawnOwner();
		if (OwningPawn == nullptr) return;
	}

	FVector Velocity = OwningPawn->GetVelocity();
	Speed = Velocity.Size2D();

	FRotator Rotation = OwningPawn->GetActorRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);

	// NEW: Calculate Pitch for Aim Offset
	FRotator AimRotation = OwningPawn->GetBaseAimRotation();
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, Rotation);
	Pitch = DeltaRotation.Pitch;
}