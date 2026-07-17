#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class USoundConcurrency;
class USoundAttenuation;

UCLASS()
class MGS_PROJECT_FIRST_API AWeaponBase : public AActor
{
    GENERATED_BODY()

public:
    AWeaponBase();
    virtual void Fire();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Components")
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Stats")
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Stats")
    float FireRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon | Stats")
    int32 MaxAmmo;

    UPROPERTY(BlueprintReadWrite, Category = "Weapon | Stats")
    int32 CurrentAmmo;

    UPROPERTY(BlueprintReadOnly, Category = "Weapon | Stats")
    bool bIsReloading;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Effects")
    UAnimMontage* FireMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Effects")
    UAnimMontage* ReloadMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Effects")
    UNiagaraSystem* MuzzleFlash;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Effects")
    UNiagaraSystem* BulletTraceSystem;

    // --- AUDIO ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Audio")
    USoundBase* FireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Audio")
    USoundConcurrency* FireSoundConcurrency;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Audio")
    USoundAttenuation* FireSoundAttenuation;

private:
    void Reload();
    void EndReload();
};