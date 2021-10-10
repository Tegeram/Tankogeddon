// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"
#include "DrawDebugHelpers.h"
#include "TankPawn.h"

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TankPawn = Cast<ATankPawn>(GetPawn());
	bShowMouseCursor = true;
}

// Called to bind functionality to input
void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::MoveForward);
	InputComponent->BindAxis("RotateRight", this, &ATankPlayerController::RotateRight);
	//InputComponent->BindAxis("MoveRight", this, &ATankPlayerController::MoveRight);
	InputComponent->BindAction("Fire", IE_Pressed, this, &ATankPlayerController::Fire);
	InputComponent->BindAction("FireSpecial", IE_Pressed, this, &ATankPlayerController::FireSpecial);

}

void ATankPlayerController::MoveForward(float InAxisValue)
{
	if (TankPawn)
	{
		TankPawn->MoveForward(InAxisValue);
	}	
}

void ATankPlayerController::RotateRight(float AxisValue)
{
	if (TankPawn)
	{
		TankPawn->RotateRight(AxisValue);
	}
}

void ATankPlayerController::Fire()
{
	if (TankPawn)
	{
		TankPawn->Fire();
	}
}

void ATankPlayerController::FireSpecial()
{
	if (TankPawn)
	{
		TankPawn->FireSpecial();
	}
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!TankPawn) 
	{
		return;
	}

	FVector WorldMousePosition, WorldMouseDirection;
	DeprojectMousePositionToWorld(WorldMousePosition, WorldMouseDirection);
	FVector TurretTargetDirection = WorldMousePosition - TankPawn->GetActorLocation();
	TurretTargetDirection.Z = 0.f;
	TurretTargetDirection.Normalize();
	FVector TurretTargetPosition = TankPawn->GetActorLocation() + TurretTargetDirection * 1000.f;
	DrawDebugLine(GetWorld(), TankPawn->GetActorLocation(), TurretTargetPosition, FColor::Green, false, 0.1f, 0, 5.f);
	TankPawn->SetTurretTargetPosition(TurretTargetPosition);
}

//void ATankPlayerController::MoveRight(float InAxisValue) 
//{
//	if (TankPawn)
//	{
//		TankPawn->MoveRight(InAxisValue);
//	}
//}
