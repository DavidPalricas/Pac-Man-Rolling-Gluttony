// Fill out your copyright notice in the Description page of Project Settings.

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

UCLASS()
class MYPROJECT_API APacManPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APacManPawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	UInputAction* MouseMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	UInputAction* Jump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	UInputAction* Exit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	TSubclassOf<UUserWidget>HUDClass;

	UPROPERTY();
	UUserWidget* HUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly);
	TSubclassOf<AActor> Cherry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* CherryCollectedSound;

	float score;

	UStaticMeshComponent* SphereMesh;
	USpringArmComponent* SpringArm;
	UCameraComponent* Camera;

	UTextBlock* scoreText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnMovement(const FInputActionValue& Value);

	void OnMouseMovement(const FInputActionValue& Value);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnCherry(FVector position);

	void HandleJump();

	void ExitGame();
};
