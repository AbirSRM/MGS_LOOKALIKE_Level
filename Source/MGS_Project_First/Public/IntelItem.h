#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntelItem.generated.h"

UCLASS()
class MGS_PROJECT_FIRST_API AIntelItem : public AActor
{
	GENERATED_BODY()

public:
	AIntelItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* IntelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* OverlapZone;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};