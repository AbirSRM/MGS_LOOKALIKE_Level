#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h" 
#include "NiagaraComponent.h" 
#include "NiagaraDataInterfaceArrayFunctionLibrary.h" 
#include "DrawDebugHelpers.h"
#include "Engine/DamageEvents.h" 
#include "AIController.h" 
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"
#include "Sound/SoundConcurrency.h" 
#include "Sound/SoundAttenuation.h" 

AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = RootComp;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Damage = 5.0f;
    FireRange = 5000.0f;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    bIsReloading = false;
}

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = MaxAmmo;
}

void AWeaponBase::Fire()
{
    if (bIsReloading) return;

    if (CurrentAmmo <= 0)
    {
        Reload();
        return;
    }

    CurrentAmmo--;

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetAttachParentActor());
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (!OwnerCharacter) return;
    }

    if (FireMontage) OwnerCharacter->PlayAnimMontage(FireMontage);

    FVector StartLoc = WeaponMesh->GetSocketLocation(FName("Muzzle"));
    FVector ForwardVector = OwnerCharacter->GetActorForwardVector();

    // FIXED: Brute Force Audio. Concurrency limits are ignored so sound plays on every enemy bullet.
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, StartLoc, 1.0f, 1.0f, 0.0f, FireSoundAttenuation);
    }

    if (MuzzleFlash)
        UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlash, WeaponMesh, FName("Muzzle"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);

    if (AAIController* AIController = Cast<AAIController>(OwnerCharacter->GetController()))
    {
        if (UBlackboardComponent* BBComp = AIController->GetBlackboardComponent())
        {
            if (AActor* Target = Cast<AActor>(BBComp->GetValueAsObject("TargetActor")))
            {
                FVector ExactDirection = (Target->GetActorLocation() - StartLoc).GetSafeNormal();
                ForwardVector = FMath::VRandCone(ExactDirection, FMath::DegreesToRadians(6.0f));
            }
        }
    }

    FVector EndLoc = StartLoc + (ForwardVector * FireRange);
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(OwnerCharacter);
    QueryParams.bTraceComplex = true;

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, QueryParams);
    FVector ActualHitLocation = bHit ? HitResult.ImpactPoint : EndLoc;

    if (bHit && HitResult.GetActor())
    {
        UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, ForwardVector, HitResult, OwnerCharacter->GetController(), this, UDamageType::StaticClass());
    }

    if (BulletTraceSystem)
    {
        UNiagaraComponent* TracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTraceSystem, StartLoc);
        if (TracerComp)
        {
            TracerComp->SetVectorParameter(FName("MuzzlePosition"), StartLoc);
            TArray<FVector> ImpactArray;
            ImpactArray.Add(ActualHitLocation);
            UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TracerComp, FName("ImpactPositions"), ImpactArray);
            TracerComp->SetBoolParameter(FName("Trigger"), true);
        }
    }
}

void AWeaponBase::Reload()
{
    bIsReloading = true;
    float ReloadTime = 2.5f;

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetAttachParentActor());
    if (!OwnerCharacter) OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (OwnerCharacter && ReloadMontage)
    {
        ReloadTime = OwnerCharacter->PlayAnimMontage(ReloadMontage);
    }

    FTimerHandle ReloadTimerHandle;
    GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponBase::EndReload, ReloadTime, false);
}

void AWeaponBase::EndReload()
{
    CurrentAmmo = MaxAmmo;
    bIsReloading = false;
}