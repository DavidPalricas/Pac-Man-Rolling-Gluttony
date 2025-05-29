// Fill out your copyright notice in the Description page of Project Settings.

#include "PacManPawn.h"
#include "EnhancedInputSubsystems.h" // Correct header for UEnhancedInputLocalPlayerSubsystem
#include "Blueprint/WidgetTree.h" 
#include "EnhancedInputComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
APacManPawn::APacManPawn()
{
// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APacManPawn::BeginPlay()
{
	Super::BeginPlay();

	// Activating the input
	if (APlayerController* playerCtrl = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerCtrl->GetLocalPlayer());

		if (subSystem)
		{
			subSystem->ClearAllMappings();
			subSystem->AddMappingContext(MappingContext, 0);
		}
	}

	SphereMesh = FindComponentByTag<UStaticMeshComponent>("PacMan");
	SphereMesh->OnComponentBeginOverlap.AddDynamic(this, &APacManPawn::OnOverlapBegin);
	SpringArm = FindComponentByClass<USpringArmComponent>();
	Camera = FindComponentByClass<UCameraComponent>();


	HUD = CreateWidget<UUserWidget>(GetWorld(), HUDClass);
	HUD->AddToViewport(0);

	UWidgetTree* widgetTree = HUD->WidgetTree.Get();
	scoreText = widgetTree->FindWidget<UTextBlock>(FName("Score"));
}

// Called every frame
void APacManPawn::Tick(float DeltaTime)
{
Super::Tick(DeltaTime);
}

// Called to bind functionality to input
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

void APacManPawn::OnMovement(const FInputActionValue& Value)
{
	FInputActionValue::Axis2D inputValue = Value.Get<FInputActionValue::Axis2D>();

	// Camera to be parralell to the ground
	FVector fowardVector = Camera->GetForwardVector();
	fowardVector.Z = 0; // Ignore vertical movement
	fowardVector.Normalize(); // Ensure the forward vector is normalized

	FVector movementVector = inputValue.X * Camera->GetForwardVector() + inputValue.Y - Camera->GetRightVector();

	SphereMesh->AddForce(FVector(inputValue.X, inputValue.Y, 0) ); // Adjust force multiplier as needed
}


void APacManPawn::OnMouseMovement(const FInputActionValue& Value)
{
	FInputActionValue::Axis2D inputValue = Value.Get<FInputActionValue::Axis2D>();

	SpringArm->AddLocalRotation(FRotator(inputValue.Y, inputValue.X, 0));

	FRotator springArmRotation = SpringArm->GetRelativeRotation();

	SpringArm->SetRelativeRotation(FRotator(springArmRotation.Pitch, springArmRotation.Yaw, 0));

	// Handle mouse movement logic here
	// For example, you could rotate the pawn based on mouse movement
	FRotator NewRotation = FRotator(0, inputValue.X * 10.0f, 0); // Adjust rotation speed as needed
	AddActorLocalRotation(NewRotation);
}

void APacManPawn::OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)  
{    
   FVector cherryPos = FVector::ZeroVector; // Initialize cherry position  

   if (OtherActor->Tags.Contains("Cherry"))  
   {
       cherryPos = OtherActor->GetActorLocation();  

	   if (CherryCollectedSound) 
	   {
		   UGameplayStatics::PlaySoundAtLocation(
			   this,              
			   CherryCollectedSound,       
			   cherryPos,         
			   1.0f,              
			   1.0f,              
			   0.0f,              
			   nullptr,          
			   nullptr            
		   );
	   }

       OtherActor->Destroy();  
       score++;  
   }  

   FString scoreString = "Score: ";
   scoreString.AppendInt(score);
   scoreText->SetText(FText::FromString(scoreString));  

   if (cherryPos != FVector::ZeroVector) {  
       FVector capturedCherryPos = cherryPos; 

       FTimerHandle TimerHandle;  
       GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, capturedCherryPos]()  
           {  
              SpawnCherry(capturedCherryPos);  
           }, 2.0f, false);  
   }  
}

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


void APacManPawn::HandleJump() {  
    FVector CurrentVelocity = SphereMesh->GetPhysicsLinearVelocity();  

    // Check if the object is on the ground  
    if (FMath::IsNearlyZero(CurrentVelocity.Z, 1.0f)) { 
       FVector JumpImpulse = FVector(0, 0, 500.0f);  
       SphereMesh->AddImpulse(JumpImpulse, NAME_None, true);  
     }  
}

void APacManPawn::ExitGame() {  
   if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {  
       PlayerController->ConsoleCommand("quit");  
   }  
}
