#include "IntelItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h" 

AIntelItem::AIntelItem()
{
	PrimaryActorTick.bCanEverTick = false;

	OverlapZone = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapZone"));
	RootComponent = OverlapZone;
	OverlapZone->SetCollisionProfileName(TEXT("Trigger"));
	OverlapZone->SetBoxExtent(FVector(50.f, 50.f, 50.f));

	IntelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IntelMesh"));
	IntelMesh->SetupAttachment(RootComponent);
	IntelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapZone->OnComponentBeginOverlap.AddDynamic(this, &AIntelItem::OnOverlapBegin);
}

void AIntelItem::BeginPlay()
{
	Super::BeginPlay();
}

void AIntelItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player)
		{
			Player->PickupIntel();
			Destroy(); // Removes intel from the map
		}
	}
}