#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h" 
#include "PlayerCharacter.generated.h"

class UNiagaraSystem;
class USoundBase;
class USoundConcurrency;
class USoundAttenuation;

UCLASS()
class MGS_PROJECT_FIRST_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // --- NEW OBJECTIVE LOGIC ---
    UFUNCTION(BlueprintCallable, Category = "Objectives")
    void PickupIntel();

    UFUNCTION(BlueprintPure, Category = "Objectives")
    bool HasIntel() const { return bHasIntel; }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class USkeletalMeshComponent* EquippedWeaponMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    class UStaticMeshComponent* ReloadMagazineMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USpringArmComponent* SpringArmComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* CameraComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    class UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Locomotion")
    class UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Locomotion")
    class UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Locomotion")
    class UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Locomotion")
    class UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat")
    class UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat")
    class UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat")
    class UInputAction* ReloadAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    class UAnimMontage* FireHipMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    class UAnimMontage* FireAimMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    class UAnimMontage* ReloadMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animations")
    class UAnimMontage* HitReactMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapons")
    float FireRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapons")
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapons")
    int32 MaxAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapons")
    int32 CurrentAmmo;

    FTimerHandle FireTimerHandle;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsFiring;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bWantsToFire;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsReloading;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsDead;

    // --- NEW OBJECTIVE TRACKER ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Objectives")
    bool bHasIntel;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    float MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float Health;

    // --- AUDIO ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundBase* FireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundConcurrency* FireSoundConcurrency;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundAttenuation* FireSoundAttenuation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundBase* DamageTakenSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundBase* DamageGivenSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Combat")
    USoundBase* ReloadSound;

    // --- FOOTSTEPS ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Movement")
    USoundBase* FootstepSound1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Movement")
    USoundBase* FootstepSound2;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Movement")
    float BaseFootstepInterval;

    float CurrentFootstepTimer;
    bool bToggleFootstep;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|VFX")
    UNiagaraSystem* MuzzleFlashSystem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|VFX")
    UNiagaraSystem* BulletTraceSystem;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|VFX")
    FName MuzzleSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Weapons")
    float WeaponRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Recoil")
    float RecoilPitch;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Recoil")
    float RecoilYaw;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> CrosshairWidgetClass;

    UPROPERTY()
    class UUserWidget* CrosshairWidgetInstance;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void ToggleCrouch();

    void StartAiming();
    void StopAiming();
    void StartFiring();
    void FireWeapon();
    void StopFiring();

    void Reload();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EndReload();

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EquipWeapon(class USkeletalMesh* NewWeaponMesh);

    UFUNCTION(BlueprintPure, Category = "Stats")
    FText GetHealthText() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int32 GetCurrentAmmo() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool GetIsReloading() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool GetIsAiming() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool GetIsDead() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ShowMagazine();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void HideMagazine();

    FORCEINLINE bool GetIsFiring() const { return bIsFiring; }
    FORCEINLINE class UCameraComponent* GetCameraComp() const { return CameraComp; }
};