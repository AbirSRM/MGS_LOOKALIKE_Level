#include "PlayerCharacter.h"
#include "UIHandler.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Math/UnrealMathUtility.h" 
#include "Animation/AnimInstance.h" 
#include "Animation/AnimMontage.h" 
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/Engine.h" 
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h" 
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h" 
#include "Components/CapsuleComponent.h"
#include "Sound/SoundConcurrency.h" 
#include "Sound/SoundAttenuation.h" 
#include "Perception/AISense_Damage.h" 
#include "Perception/AISense_Hearing.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    EquippedWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EquippedWeaponMesh"));
    EquippedWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EquippedWeaponMesh->SetupAttachment(GetMesh(), FName("Weapon_Socket"));

    ReloadMagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReloadMagazineMesh"));
    ReloadMagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ReloadMagazineMesh->SetupAttachment(GetMesh(), FName("hand_l"));
    ReloadMagazineMesh->SetVisibility(false);

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->TargetArmLength = 280.0f;
    SpringArmComp->SocketOffset = FVector(0.0f, 45.0f, 85.0f);
    SpringArmComp->bEnableCameraLag = true;
    SpringArmComp->CameraLagSpeed = 12.0f;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

    GetCharacterMovement()->MaxWalkSpeed = 400.0f;

    bIsAiming = false;
    bIsFiring = false;
    bWantsToFire = false;
    bIsReloading = false;
    bIsDead = false;
    bHasIntel = false;

    FireRate = 0.15f;
    BaseDamage = 20.0f;
    WeaponRange = 10000.f;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;

    MaxHealth = 100.0f;
    Health = MaxHealth;

    MuzzleSocketName = FName("Main_Muzzle");
    RecoilPitch = -0.5f;
    RecoilYaw = 0.2f;

    BaseFootstepInterval = 0.35f;
    CurrentFootstepTimer = 0.0f;
    bToggleFootstep = true;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
    CurrentAmmo = MaxAmmo;
    bIsDead = false;
    bHasIntel = false;

    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    if (CrosshairWidgetClass)
    {
        CrosshairWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
        if (CrosshairWidgetInstance)
        {
            CrosshairWidgetInstance->AddToViewport();
            CrosshairWidgetInstance->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float TargetArmLength = bIsAiming ? 130.0f : 280.0f;
    float TargetY = bIsAiming ? 75.0f : 45.0f;
    float TargetZ = bIsCrouched ? (bIsAiming ? 60.0f : 45.0f) : (bIsAiming ? 75.0f : 85.0f);

    FVector TargetOffset = FVector(0.0f, TargetY, TargetZ);

    // --- THIS IS THE FIXED LINE ---
    SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, TargetArmLength, DeltaTime, 12.0f);

    SpringArmComp->SocketOffset = FMath::VInterpTo(SpringArmComp->SocketOffset, TargetOffset, DeltaTime, 12.0f);

    bool bIsMovingOnGround = GetCharacterMovement()->IsMovingOnGround() && GetVelocity().SizeSquared2D() > 100.0f;

    if (!bIsDead && bIsMovingOnGround)
    {
        CurrentFootstepTimer -= DeltaTime;
        if (CurrentFootstepTimer <= 0.0f)
        {
            USoundBase* SoundToPlay = bToggleFootstep ? FootstepSound1 : FootstepSound2;
            if (SoundToPlay)
            {
                UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation(), 0.5f);
            }
            bToggleFootstep = !bToggleFootstep;
            CurrentFootstepTimer = bIsCrouched ? (BaseFootstepInterval * 1.5f) : BaseFootstepInterval;
        }
    }
    else
    {
        CurrentFootstepTimer = 0.0f;
    }
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) { Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust); }
void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) { Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust); }

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction) EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        if (LookAction) EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
            EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
        }
        if (CrouchAction) EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleCrouch);
        if (AimAction)
        {
            EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &APlayerCharacter::StartAiming);
            EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopAiming);
        }
        if (FireAction)
        {
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerCharacter::StartFiring);
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopFiring);
        }
        if (ReloadAction) EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &APlayerCharacter::Reload);
    }
}

void APlayerCharacter::ToggleCrouch() { if (!bIsDead) bIsCrouched ? UnCrouch() : Crouch(); }

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    if (bIsDead) return;
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MovementVector.Y);
        AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MovementVector.X);
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    if (bIsDead) return;
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void APlayerCharacter::EquipWeapon(USkeletalMesh* NewWeaponMesh) { if (EquippedWeaponMesh && NewWeaponMesh) EquippedWeaponMesh->SetSkeletalMeshAsset(NewWeaponMesh); }

void APlayerCharacter::StartAiming()
{
    if (bIsReloading || bIsDead) return;
    bIsAiming = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bUseControllerRotationYaw = true;
}

void APlayerCharacter::StopAiming()
{
    bIsAiming = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
}

void APlayerCharacter::StartFiring()
{
    if (bIsDead) return;
    bWantsToFire = true;
    bIsFiring = true;

    if (!bIsAiming && !bIsReloading)
    {
        GetCharacterMovement()->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;
    }

    if (!GetWorldTimerManager().IsTimerActive(FireTimerHandle))
    {
        FireWeapon();
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerCharacter::FireWeapon, FireRate, true);
    }
}

void APlayerCharacter::FireWeapon()
{
    if (!bWantsToFire || CurrentAmmo <= 0 || bIsReloading || bIsDead)
    {
        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        bIsFiring = false;

        if (CurrentAmmo <= 0 && !bIsReloading && !bIsDead && bWantsToFire)
        {
            Reload();
        }
        return;
    }

    CurrentAmmo--;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        UAnimMontage* MontageToPlay = bIsAiming ? FireAimMontage : FireHipMontage;
        if (MontageToPlay) AnimInstance->Montage_Play(MontageToPlay);
    }

    FVector MuzzleLocation = EquippedWeaponMesh ? EquippedWeaponMesh->GetSocketLocation(MuzzleSocketName) : GetActorLocation();

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation, 1.0f, 1.0f, 0.0f, FireSoundAttenuation);
    }

    UAISense_Hearing::ReportNoiseEvent(GetWorld(), MuzzleLocation, 1.0f, this, 3000.0f, TEXT("WeaponFire"));

    if (MuzzleFlashSystem && EquippedWeaponMesh)
        UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashSystem, EquippedWeaponMesh, MuzzleSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);

    if (CameraComp)
    {
        FVector CameraLocation = CameraComp->GetComponentLocation();
        FVector CameraForward = CameraComp->GetForwardVector();
        FVector TraceStart = CameraLocation + (CameraForward * 250.0f);
        FVector TraceEnd = TraceStart + (CameraForward * WeaponRange);

        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.bTraceComplex = true;

        FHitResult HitResult;
        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);
        FVector ActualHitLocation = bHit ? HitResult.ImpactPoint : TraceEnd;

        if (bHit && HitResult.GetActor())
        {
            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), BaseDamage, CameraForward, HitResult, GetController(), this, UDamageType::StaticClass());

            UAISense_Damage::ReportDamageEvent(GetWorld(), HitResult.GetActor(), this, BaseDamage, ActualHitLocation, ActualHitLocation);

            if (DamageGivenSound)
            {
                UGameplayStatics::PlaySoundAtLocation(this, DamageGivenSound, ActualHitLocation);
            }
        }

        if (EquippedWeaponMesh && BulletTraceSystem)
        {
            UNiagaraComponent* TracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletTraceSystem, MuzzleLocation);
            if (TracerComp)
            {
                TracerComp->SetVectorParameter(FName("MuzzlePosition"), MuzzleLocation);
                TArray<FVector> ImpactArray;
                ImpactArray.Add(ActualHitLocation);
                UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TracerComp, FName("ImpactPositions"), ImpactArray);
                TracerComp->SetBoolParameter(FName("Trigger"), true);
            }
        }
    }

    if (Controller != nullptr)
    {
        AddControllerPitchInput(FMath::RandRange(RecoilPitch, RecoilPitch * 0.5f));
        AddControllerYawInput(FMath::RandRange(-RecoilYaw, RecoilYaw));
    }
}

void APlayerCharacter::StopFiring()
{
    bWantsToFire = false;

    if (!bIsAiming && !bIsDead)
    {
        GetCharacterMovement()->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;
    }
}

void APlayerCharacter::Reload()
{
    if (CurrentAmmo == MaxAmmo || bIsReloading || bIsDead) return;

    bIsReloading = true;
    StopAiming();

    if (ReloadSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && ReloadMontage)
    {
        float MontageLength = AnimInstance->Montage_Play(ReloadMontage);
        FTimerHandle ReloadTimerHandle;
        GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &APlayerCharacter::EndReload, MontageLength, false);
    }
    else
    {
        EndReload();
    }
}

void APlayerCharacter::EndReload()
{
    if (bIsDead) return;
    CurrentAmmo = MaxAmmo;
    bIsReloading = false;
    if (bWantsToFire) StartFiring();
}

FText APlayerCharacter::GetHealthText() const
{
    FString HealthString = FString::Printf(TEXT("%.0f"), Health);
    return FText::FromString(HealthString);
}

int32 APlayerCharacter::GetCurrentAmmo() const { return CurrentAmmo; }
bool APlayerCharacter::GetIsReloading() const { return bIsReloading; }
bool APlayerCharacter::GetIsAiming() const { return bIsAiming; }
bool APlayerCharacter::GetIsDead() const { return bIsDead; }
void APlayerCharacter::ShowMagazine() { if (ReloadMagazineMesh) ReloadMagazineMesh->SetVisibility(true); }
void APlayerCharacter::HideMagazine() { if (ReloadMagazineMesh) ReloadMagazineMesh->SetVisibility(false); }

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.0f;

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser) * 0.5f;

    if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)&DamageEvent;
        FName HitBoneName = PointDamageEvent->HitInfo.BoneName;

        if (HitBoneName == "head" || HitBoneName == "neck_01") ActualDamage *= 2.0f;
        else if (HitBoneName.ToString().Contains("arm") || HitBoneName.ToString().Contains("hand")) ActualDamage *= 0.5f;
        else if (HitBoneName.ToString().Contains("leg") || HitBoneName.ToString().Contains("foot") || HitBoneName.ToString().Contains("calf") || HitBoneName.ToString().Contains("thigh")) ActualDamage *= 0.5f;
        else ActualDamage *= 1.0f;
    }

    Health -= ActualDamage;

    if (DamageTakenSound && Health > 0.0f)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DamageTakenSound, GetActorLocation());
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (Health <= 0.0f)
    {
        Health = 0.0f;
        bIsDead = true;

        GetWorldTimerManager().ClearTimer(FireTimerHandle);
        bIsFiring = false;

        // --- ADDED UI CONNECTION HERE ---
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            DisableInput(PC);

            if (AUIHandler* UIController = Cast<AUIHandler>(PC))
            {
                UIController->ShowDeathScreen();
            }
        }

        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

// --- UPDATED OBJECTIVE FUNCTION WITH DEBUG BRIDGES ---
void APlayerCharacter::PickupIntel()
{
    bHasIntel = true;

    // 1. Print to Output Log
    UE_LOG(LogTemp, Warning, TEXT(">>> PickupIntel() Called on PlayerCharacter <<<"));

    // 2. Check what Controller is currently possessing this pawn
    if (GEngine)
    {
        FString ControllerName = GetController() ? GetController()->GetClass()->GetName() : TEXT("NULL CONTROLLER");
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Current Controller Class: %s"), *ControllerName));
    }

    // 3. Attempt the cast and trigger UI
    if (AUIHandler* UIController = Cast<AUIHandler>(GetController()))
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SUCCESS: Cast to AUIHandler worked. Calling ShowIntelSecuredPopup()."));

        UIController->ShowIntelSecuredPopup();
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FAILURE: Cast to AUIHandler failed. Check GameMode Settings!"));
    }
}