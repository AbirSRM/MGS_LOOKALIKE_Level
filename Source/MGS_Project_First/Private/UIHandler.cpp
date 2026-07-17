#include "UIHandler.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AUIHandler::BeginPlay()
{
	Super::BeginPlay();

	// Add the UI input mapping context so the controller can listen for the Pause action
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (UIContext)
		{
			Subsystem->AddMappingContext(UIContext, 1);
		}
	}

	// Spawn the Start Screen immediately when the level loads
	if (StartScreenClass)
	{
		StartScreenWidget = CreateWidget<UUserWidget>(this, StartScreenClass);
		if (StartScreenWidget)
		{
			StartScreenWidget->AddToViewport();

			// Show the mouse cursor and set input to UI only so the player can click "Start"
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AUIHandler::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind the Pause action directly to the UI Handler
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AUIHandler::TogglePauseMenu);
		}
	}
}

void AUIHandler::StartGameplay()
{
	// Remove the start screen and return control to the player
	if (StartScreenWidget)
	{
		StartScreenWidget->RemoveFromParent();
		StartScreenWidget = nullptr;
	}

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AUIHandler::TogglePauseMenu()
{
	if (IsPaused())
	{
		ResumeGame();
	}
	else
	{
		if (PauseMenuClass)
		{
			ActivePauseMenu = CreateWidget<UUserWidget>(this, PauseMenuClass);
			if (ActivePauseMenu)
			{
				ActivePauseMenu->AddToViewport();
				SetPause(true);
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void AUIHandler::ResumeGame()
{
	if (ActivePauseMenu)
	{
		ActivePauseMenu->RemoveFromParent();
		ActivePauseMenu = nullptr;
	}
	SetPause(false);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void AUIHandler::ShowDeathScreen()
{
	if (DeathScreenClass)
	{
		UUserWidget* DeathWidget = CreateWidget<UUserWidget>(this, DeathScreenClass);
		if (DeathWidget)
		{
			DeathWidget->AddToViewport();
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AUIHandler::ShowMissionCompleteScreen()
{
	if (MissionCompleteClass)
	{
		UUserWidget* MissionCompleteWidget = CreateWidget<UUserWidget>(this, MissionCompleteClass);
		if (MissionCompleteWidget)
		{
			MissionCompleteWidget->AddToViewport();
			SetPause(true);
			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}
	}
}

void AUIHandler::ShowIntelSecuredPopup()
{
	if (IntelSecuredClass)
	{
		UUserWidget* PopupWidget = CreateWidget<UUserWidget>(this, IntelSecuredClass);
		if (PopupWidget)
		{
			PopupWidget->AddToViewport();
		}
	}
}