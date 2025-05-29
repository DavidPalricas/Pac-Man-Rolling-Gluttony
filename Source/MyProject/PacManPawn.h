/**
 * @file PacManPawn.h
 * @brief Header file for APacManPawn class - Pac-Man game character in Unreal Engine
 * @author [David Palricas]
 * @date [29-05-2025]
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Camera/CameraComponent.h>
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include "PacManPawn.generated.h"

/**
 * @class APacManPawn
 * @brief Main pawn class for the Pac-Man character
 * 
 * This class handles the Pac-Man player character, including movement, input handling,
 * collision detection with cherries, score management, and UI updates.
 * Inherits from APawn to provide basic pawn functionality in Unreal Engine.
 */
UCLASS()
class MYPROJECT_API APacManPawn : public APawn
{
    GENERATED_BODY()

public:
    /**
     * @brief Default constructor
     * 
     * Sets default values for this pawn's properties and initializes components.
     */
    APacManPawn();

    /** @brief Input action for player movement (WASD/Arrow keys) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* MovementAction;

    /** @brief Input action for mouse movement and camera control */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* MouseMovement;

    /** @brief Input action for jumping */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* Jump;

    /** @brief Input action for exiting the game */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputAction* Exit;

    /** @brief Input mapping context for enhanced input system */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UInputMappingContext* MappingContext;

    /** @brief Class reference for the HUD widget blueprint */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UUserWidget> HUDClass;

    /** @brief Instance of the HUD widget displayed on screen */
    UPROPERTY()
    UUserWidget* HUD;

    /** @brief Class reference for cherry actor blueprint */
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AActor> Cherry;

    /** @brief Sound effect played when collecting a cherry */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
    USoundBase* CherryCollectedSound;

    /** @brief Current player score */
    float score;

    /** @brief Static mesh component representing the Pac-Man sphere */
    UStaticMeshComponent* SphereMesh;

    /** @brief Spring arm component for camera positioning and rotation */
    USpringArmComponent* SpringArm;

    /** @brief Camera component for player view */
    UCameraComponent* Camera;

    /** @brief Text block widget displaying the current score */
    UTextBlock* scoreText;

protected:
    /**
     * @brief Called when the game starts or when spawned
     * 
     * Initializes input system, components, and UI elements.
     */
    virtual void BeginPlay() override;

public:
    /**
     * @brief Called every frame
     * 
     * @param DeltaTime Time elapsed since the last frame
     */
    virtual void Tick(float DeltaTime) override;

    /**
     * @brief Called to bind functionality to input
     * 
     * Sets up input bindings for movement, mouse control, jumping, and exiting.
     * 
     * @param PlayerInputComponent Input component to bind actions to
     */
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /**
     * @brief Handles player movement input
     * 
     * Processes WASD/arrow key input and applies movement forces to the pawn.
     * 
     * @param Value Input action value containing movement direction
     */
    void OnMovement(const FInputActionValue& Value);

    /**
     * @brief Handles mouse movement input
     * 
     * Controls camera rotation and pawn orientation based on mouse movement.
     * 
     * @param Value Input action value containing mouse delta movement
     */
    void OnMouseMovement(const FInputActionValue& Value);

    /**
     * @brief Overlap event handler for collision detection
     * 
     * Called when the pawn overlaps with another object. Handles cherry collection,
     * score updates, sound effects, and cherry respawning.
     * 
     * @param OverlappedComponent The component that was overlapped
     * @param OtherActor The actor that caused the overlap
     * @param OtherComp The other actor's component involved in the overlap
     * @param OtherBodyIndex Index of the other object's body
     * @param bFromSweep Whether the overlap resulted from a sweep
     * @param SweepResult Hit result information from the sweep
     */
    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, 
                       class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
                       const FHitResult& SweepResult);

    /**
     * @brief Spawns a new cherry at the specified location
     * 
     * Creates a new cherry actor instance and adds appropriate tags.
     * 
     * @param position World position where the cherry should be spawned
     */
    void SpawnCherry(FVector position);

    /**
     * @brief Handles jump input
     * 
     * Applies vertical impulse to make the pawn jump if it's on the ground.
     */
    void HandleJump();

    /**
     * @brief Exits the game
     * 
     * Executes quit command to close the application.
     */
    void ExitGame();
};