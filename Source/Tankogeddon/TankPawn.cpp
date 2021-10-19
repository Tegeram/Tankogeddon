// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "TankPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"
#include "Cannon.h"

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

	SetupCannon(DefaultCannonClass);
}



// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//CurrentMoveForwardAxis = FMath::Lerp(CurrentMoveForwardAxis, TargetMoveForwardAxis, MovementSmootheness);
	CurrentMoveForwardAxis = FMath::FInterpTo(CurrentMoveForwardAxis, TargetMoveForwardAxis, DeltaTime, MovementSmootheness);
	FVector MoveVector = GetActorForwardVector() * CurrentMoveForwardAxis;
	FVector NewActorLocation = GetActorLocation() + MoveVector * MoveSpeed * DeltaTime;
	SetActorLocation(NewActorLocation, true);
	
	//CurrentRotateRightAxis = FMath::Lerp(CurrentRotateRightAxis, TargetRotateRightAxis, RotationSmootheness);
	CurrentRotateRightAxis = FMath::FInterpTo(CurrentRotateRightAxis, TargetRotateRightAxis, DeltaTime, RotationSmootheness);
	float Rotation = GetActorRotation().Yaw + RotationSpeed * CurrentRotateRightAxis * DeltaTime;
	SetActorRotation(FRotator(0, Rotation, 0));

	//UE_LOG(LogTankoggedon, Verbose, TEXT("CurrentRightAxisValue: %f"), CurrentRightAxisValue);

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TurretTargetPosition);
	FRotator CurrentRotation = TurretMesh->GetComponentRotation();
	TargetRotation.Roll = CurrentRotation.Roll;
	TargetRotation.Pitch = CurrentRotation.Pitch;
	//TurretMesh->SetWorldRotation(FMath::Lerp(CurrentRotation, TargetRotation, TurretRotationSmootheness));
	TurretMesh->SetWorldRotation(FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaTime, TurretRotationSmootheness));
	
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
	if (SelectedCannon)
	{
		SelectedCannon->Fire();
	}
}

void ATankPawn::FireSpecial()
{
	if (SelectedCannon)
	{
		SelectedCannon->FireSpecial();
	}
}

void ATankPawn::SetupCannon(TSubclassOf<class ACannon> InCannonClass)
{
	if (SelectedCannon)
	{
		SelectedCannon->Destroy();
	}

	if (InCannonClass)
	{
		FActorSpawnParameters Params;
		Params.Instigator = this;
		Params.Owner = this;
		SelectedCannon = GetWorld()->SpawnActor<ACannon>(InCannonClass, Params);
		SelectedCannon->AttachToComponent(CannonSpawnPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
}

void  ATankPawn::SwitchCannon() //òîëüêî íå ïîíÿë êàê ñäåëàòü ñìåíó òðåõ ïóøêåê 
{
	Swap(SelectedCannon, UnselectedCannon);
	if (SelectedCannon)
	{
		SelectedCannon->SetVisibility(true);
	}

	if (UnselectedCannon)
	{
		UnselectedCannon->SetVisibility(false);
	}
}

ACannon* ATankPawn::GetSelectedCannon() const
{
	return SelectedCannon;
}

oid ATurret::OnHealthChanged_Implementation(float Damage)
{
	UE_LOG(LogTankogeddon, Log, TEXT("Turret %s taked damage:%f "), *GetName(), Damage);
}

void ATurret::OnDie_Implementation()
{
	Destroy();
}

void ATurret::TakeDamage(const FDamageData& DamageData)
{
	HealthComponent->TakeDamage(DamageData);
}

