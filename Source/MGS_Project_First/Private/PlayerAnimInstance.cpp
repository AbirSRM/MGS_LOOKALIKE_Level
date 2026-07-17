#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (PlayerCharacter)
    {
        // Locomotion
        FVector Velocity = PlayerCharacter->GetVelocity();
        Velocity.Z = 0.0f;
        Speed = Velocity.Size();

        FRotator ActorRotation = PlayerCharacter->GetActorRotation();
        Direction = UKismetAnimationLibrary::CalculateDirection(PlayerCharacter->GetVelocity(), ActorRotation);

        bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
        bIsCrouching = PlayerCharacter->bIsCrouched;

        // Combat States
        bIsAiming = PlayerCharacter->GetIsAiming();
        bIsFiring = PlayerCharacter->GetIsFiring();

        // Calculate AimPitch for Aim Offset 1D
        FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
        FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);
        AimPitch = FMath::Clamp(DeltaRot.Pitch, -90.0f, 90.0f);

        // Calculate Procedural Crosshair Aiming (Line Trace)
        if (UCameraComponent* PlayerCam = PlayerCharacter->GetCameraComp())
        {
            FVector TraceStart = PlayerCam->GetComponentLocation();
            // Project the trace forward by 10,000 units (100 meters)
            FVector TraceEnd = TraceStart + (PlayerCam->GetForwardVector() * 10000.0f);

            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(PlayerCharacter); // Don't hit our own capsule/mesh

            // Execute the trace
            if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
            {
                // We hit a wall/enemy, look exactly at the impact point
                AimTargetLocation = HitResult.ImpactPoint;
            }
            else
            {
                // We hit nothing, look straight out into the distance
                AimTargetLocation = TraceEnd;
            }
        }
    }
}