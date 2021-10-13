// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TANKOGEDDON_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

public:	
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

private:

	void MoveForward(float InAxisValue);
	//void MoveRight(float inAxisValue);
	void RotateRight(float AxisValue);
	void Fire();
	void FireSpecial();
	
	UPROPERTY()
	class ATankPawn* TankPawn;


};
