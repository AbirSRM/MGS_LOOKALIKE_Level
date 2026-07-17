#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "InputActionValue.h"
#include "UIHandler.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class MGS_PROJECT_FIRST_API AUIHandler : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Call these to spawn/toggle the respective screens
	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowDeathScreen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMissionCompleteScreen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowIntelSecuredPopup();

	// Helper to resume game from UMG buttons
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ResumeGame();

	// Helper to start the game and remove the start screen
	UFUNCTION(BlueprintCallable, Category = "UI")
	void StartGameplay();

protected:
	// Assign these in your BP_UIHandler defaults
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StartScreenClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DeathScreenClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MissionCompleteClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> IntelSecuredClass;

	// --- NEW INPUT BINDINGS FOR PAUSING ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* UIContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PauseAction;

private:
	UPROPERTY()
	UUserWidget* StartScreenWidget;

	UPROPERTY()
	UUserWidget* ActivePauseMenu;
};