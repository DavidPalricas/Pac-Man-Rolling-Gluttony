/**
 * @file PacManPawn.cpp
 * @brief Implementation of APacManPawn class for handling the players' input logic (PacMan).
 * @author [David Palricas]
 * @date [29-05-2025]
 */

#include "PacManPawn.h"
#include "EnhancedInputSubsystems.h" // Correct header for UEnhancedInputLocalPlayerSubsystem
#include "Blueprint/WidgetTree.h" 
#include "EnhancedInputComponent.h"
#include <Kismet/GameplayStatics.h>

/**
 * @brief Default constructor for APacManPawn class
 * 
 * Initializes the Pac-Man pawn with default settings.
 * Sets the pawn to call Tick every frame.
 */
APacManPawn::APacManPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

/**
 * @brief Function called when the game starts or when spawned
 * 
 * Sets up the input system, initializes mesh components, camera and spring arm,
 * creates and configures the user interface (HUD), and finds the score text component.
 */
void APacManPawn::BeginPlay()
{
    Super::BeginPlay();

    // Activating the input system
    if (APlayerController* playerCtrl = Cast<APlayerController>(GetController()))
    {
        UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerCtrl->GetLocalPlayer());

        if (subSystem)
        {
            subSystem->ClearAllMappings();
            subSystem->AddMappingContext(MappingContext, 0);
        }
    }

    // Component initialization
    SphereMesh = FindComponentByTag<UStaticMeshComponent>("PacMan");
    SphereMesh->OnComponentBeginOverlap.AddDynamic(this, &APacManPawn::OnOverlapBegin);
    SpringArm = FindComponentByClass<USpringArmComponent>();
    Camera = FindComponentByClass<UCameraComponent>();

    // User interface creation and configuration
    HUD = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
    HUD->AddToViewport(0);

    UWidgetTree* widgetTree = HUD->WidgetTree.Get();
    scoreText = widgetTree->FindWidget<UTextBlock>(FName("Score"));
}

/**
 * @brief Function called every frame
 * 
 * @param DeltaTime Time elapsed since the last frame
 */
void APacManPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

/**
 * @brief Sets up player input component bindings
 * 
 * Binds input actions (movement, mouse, jump, exit) to their corresponding functions.
 * 
 * @param PlayerInputComponent Player's input component
 */
void APacManPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* inputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        inputComp->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APacManPawn::OnMovement);
        inputComp->BindAction(MouseMovement, ETriggerEvent::Triggered, this, &APacManPawn::OnMouseMovement);
        inputComp->BindAction(Jump, ETriggerEvent::Triggered, this, &APacManPawn::HandleJump);
        inputComp->BindAction(Exit, ETriggerEvent::Triggered, this, &APacManPawn::ExitGame);
    }
}

/**
 * @brief Handles Pac-Man movement based on player input
 * 
 * Calculates movement vector based on camera direction and applies force to the sphere mesh.
 * Movement is adjusted to be parallel to the ground (Z component zeroed).
 * 
 * @param Value Movement input value (2D axis)
 */
void APacManPawn::OnMovement(const FInputActionValue& Value)
{
    FInputActionValue::Axis2D inputValue = Value.Get<FInputActionValue::Axis2D>();

    // Camera parallel to the ground
    FVector fowardVector = Camera->GetForwardVector();
    fowardVector.Z = 0; // Ignore vertical movement
    fowardVector.Normalize(); // Ensure the forward vector is normalized

    FVector movementVector = inputValue.X * Camera->GetForwardVector() + inputValue.Y - Camera->GetRightVector();

    // Apply force based on input (adjust force multiplier as needed)
    SphereMesh->AddForce(FVector(inputValue.X, inputValue.Y, 0));
}

/**
 * @brief Handles mouse movement for camera and pawn rotation
 * 
 * Rotates the spring arm based on mouse movement and also rotates the pawn locally.
 * 
 * @param Value Mouse input value (2D axis)
 */
void APacManPawn::OnMouseMovement(const FInputActionValue& Value)
{
    FInputActionValue::Axis2D inputValue = Value.Get<FInputActionValue::Axis2D>();

    // Rotate spring arm based on mouse movement
    SpringArm->AddLocalRotation(FRotator(inputValue.Y, inputValue.X, 0));

    FRotator springArmRotation = SpringArm->GetRelativeRotation();
    SpringArm->SetRelativeRotation(FRotator(springArmRotation.Pitch, springArmRotation.Yaw, 0));

    // Rotate pawn based on horizontal mouse movement
    FRotator NewRotation = FRotator(0, inputValue.X * 10.0f, 0); // Adjust rotation speed as needed
    AddActorLocalRotation(NewRotation);
}

/**
 * @brief Function called when overlap with another object occurs
 * 
 * Detects collision with objects tagged as "Cherry", plays its collect sound and destory it.
 * The score is incremented after the cherry destruiction and the UI is updated to show the score valueu.
 * After the cherry is destroyed a new one is created after 2 seconds in its position.
 * 
 * @param OverlappedComponent Component that was overlapped
 * @param OtherActor Actor that caused the overlap
 * @param OtherComp Other actor's component
 * @param OtherBodyIndex Other object's body index
 * @param bFromSweep Whether the overlap came from a sweep
 * @param SweepResult Sweep result (if applicable)
 */
void APacManPawn::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, 
                                 class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
                                 const FHitResult & SweepResult)  
{    
    // Check if the collided object is a cherry
    if (OtherActor->Tags.Contains("Cherry"))  
    {
        FVector cherryPos = OtherActor->GetActorLocation();  

        // Play cherry collection sound if available
        if (CherryCollectedSound) 
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,              
                CherryCollectedSound,       
                cherryPos,         
                1.0f,              // Volume
                1.0f,              // Pitch
                0.0f,              // Start time
                nullptr,          
                nullptr            
            );
        }

        OtherActor->Destroy();  
        score++;  

		// Update score text in the interface
		FString scoreString = "Score: ";
		scoreString.AppendInt(score);
		scoreText->SetText(FText::FromString(scoreString));  

		FVector capturedCherryPos = cherryPos; 

		FTimerHandle TimerHandle;  
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, capturedCherryPos]()  
		{  
			SpawnCherry(capturedCherryPos);  
		}, 2.0f, false);  
    }  
}

/**
 * @brief Spawns a new cherry at the specified position
 * 
 * Creates a new cherry actor instance at the provided position.
 * 
 * @param position Position where the cherry should be created
 */
void APacManPawn::SpawnCherry(FVector position)
{
    FActorSpawnParameters spawnParams;
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    AActor* cherry = GetWorld()->SpawnActor<AActor>(Cherry, position, FRotator::ZeroRotator, spawnParams);

    if (cherry)
    {
        cherry->Tags.Add("Cherry");
    }
}

/**
 * @brief Handles Pac-Man's jump action
 * 
 * Checks if the object is on the ground (Z velocity near zero) and applies
 * a vertical impulse to simulate jumping, to avoid jumping in the air.
 */
void APacManPawn::HandleJump() 
{  
    FVector CurrentVelocity = SphereMesh->GetPhysicsLinearVelocity();  

    // Check if the object is on the ground (vertical velocity near zero)
    if (FMath::IsNearlyZero(CurrentVelocity.Z, 1.0f)) 
    { 
        FVector JumpImpulse = FVector(0, 0, 500.0f);  
        SphereMesh->AddImpulse(JumpImpulse, NAME_None, true);  
    }  
}

/**
 * @brief Exits the game
 * 
 * Exist from the game (build) or the editor
 */
void APacManPawn::ExitGame() 
{  
    if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) 
    {  
        PlayerController->ConsoleCommand("quit");  
    }  
}