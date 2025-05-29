// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Camera/CameraComponent.h>
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
	UInputMappingContext* MappingContext;


	UStaticMeshComponent* SphereMesh;
	USpringArmComponent* SpringArm;
	UCameraComponent* Camera;


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
};
