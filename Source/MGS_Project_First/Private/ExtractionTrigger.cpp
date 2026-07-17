#include "ExtractionTrigger.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "UIHandler.h"

AExtractionTrigger::AExtractionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	ExtractionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ExtractionZone"));
	RootComponent = ExtractionZone;
	ExtractionZone->SetCollisionProfileName(TEXT("Trigger"));

	ExtractionZone->OnComponentBeginOverlap.AddDynamic(this, &AExtractionTrigger::OnOverlapBegin);
}

void AExtractionTrigger::BeginPlay()
{
	Super::BeginPlay();
}

void AExtractionTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		if (Player && Player->HasIntel()) // Checks if player picked up the intel
		{
			if (AUIHandler* UIController = Cast<AUIHandler>(Player->GetController()))
			{
				UIController->ShowMissionCompleteScreen();
			}
		}
	}
}