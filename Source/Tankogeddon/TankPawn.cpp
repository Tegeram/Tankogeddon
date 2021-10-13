// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Cannon.h"

//DECLARE_LOG_CATEGORY_EXTERN(TankLog, All, All);
//DEFINE_LOG_CATEGORY(TankLog);

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank body"));
	RootComponent = BodyMesh;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank turret"));
	TurretMesh->SetupAttachment(BodyMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CannonSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn point"));
	CannonSpawnPoint->SetupAttachment(TurretMesh);
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	SetupCannon();
}



// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentMoveForwardAxis = FMath::Lerp(CurrentMoveForwardAxis, TargetMoveForwardAxis, MovementSmootheness);
	FVector MoveVector = GetActorForwardVector() * CurrentMoveForwardAxis;
	FVector NewActorLocation = GetActorLocation() + MoveVector * MoveSpeed * DeltaTime;
	SetActorLocation(NewActorLocation);	
	
	CurrentRotateRightAxis = FMath::Lerp(CurrentRotateRightAxis, TargetRotateRightAxis, RotationSmootheness);
	float Rotation = GetActorRotation().Yaw + RotationSpeed * CurrentRotateRightAxis * DeltaTime;
	SetActorRotation(FRotator(0, Rotation, 0));

	//UE_LOG(LogTankoggedon, Verbose, TEXT("CurrentRightAxisValue: %f"), CurrentRightAxisValue);

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TurretTargetPosition);
	FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	TargetRotation.Roll = CurrentRotation.Roll;
	TargetRotation.Pitch = CurrentRotation.Pitch;
	TurretMesh->SetWorldRotation(FMath::Lerp(CurrentRotation, TargetRotation, TurretRotationSmootheness));
	
	//FVector NewActorRotation = GetActorLocation() + GetActorRightVector() * MoveRightSpeed * TargetMoveRightAxis * DeltaTime;
	//SetActorLocation(NewActorRotation);
}

void ATankPawn::MoveForward(float InAxisValue)
{
	TargetMoveForwardAxis = InAxisValue;
}

void ATankPawn::RotateRight(float InAxisValue)
{
	TargetRotateRightAxis = InAxisValue;
}

void ATankPawn::SetTurretTargetPosition(const FVector& TargetPosition)
{
	TurretTargetPosition = TargetPosition;
}

void ATankPawn::Fire()
{
	if (Cannon)
	{
		if (FireCount > 0)
		{
			Cannon->Fire();
		
			FireCount -= 1;
		}
		else
		{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Green, TEXT("Count of fire is empty"));
		}
	}
}

void ATankPawn::FireSpecial()
{
	if (Cannon)
	{
		if (FireCount > 0)
		{
			Cannon->FireSpecial();

			FireCount -= 1;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Green, TEXT("Count of fire is empty"));
		}
	}
}

void ATankPawn::SetupCannon()
{
	if (Cannon)
	{
		Cannon->Destroy();
	}

	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.Owner = this;
	Cannon = GetWorld()->SpawnActor<ACannon>(DefaultCannonClass, Params);
	Cannon->AttachToComponent(CannonSpawnPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

//void ATankPawn::MoveRight(float InAxisValue)
//{
//	TargetMoveRightAxis = InAxisValue;
//}

