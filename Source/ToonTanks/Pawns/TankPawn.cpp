// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "../Controllers/TTPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"


// Called when the game starts or when spawned
ATankPawn::ATankPawn() {
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(ATankPawn::TurretMesh);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Controlled Camera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);

	bReplicates = true;
}

void ATankPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	DOREPLIFETIME(ATankPawn, MovementDirection);
	DOREPLIFETIME(ATankPawn, PlayerRotation);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
}

void ATankPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}


/* Multicast function for setting dynamic material based on TeamID */
void ATankPawn::SetTeamColour_Server_Implementation(int Team)
{
	if (TankMaterial)
	{
		UMaterialInstanceDynamic* TeamTankMat = UMaterialInstanceDynamic::Create(TankMaterial, this);

		//Switch on TeamID to get team colour
		switch (Team)
		{
		case 0:
			TeamTankMat->SetTextureParameterValue("Texture", RedTeamTexture);
			break;
		case 1:
			TeamTankMat->SetTextureParameterValue("Texture", BlueTeamTexture);
			break;

		}


		//Apply the dynamic material to the meshes.
		BaseMesh->SetMaterial(0, TeamTankMat);
		TurretMesh->SetMaterial(0, TeamTankMat);

	}
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotatePlayer(PlayerRotation);
	MovePlayer();

	if (TankController)
	{
		if (MouseFloat > 0.001 || MouseFloat < -0.001) RotateTurret(MouseFloat);
		
		
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT BINDINGS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATankPawn::GetMoveDirection);
	PlayerInputComponent->BindAxis("Turn", this, &ATankPawn::GetMoveRotation);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATankPawn::Fire);
	PlayerInputComponent->BindAxis("RotateTurret", this, &ATankPawn::GetTurretRotation);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATankPawn::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATankPawn::StopJump); //TODO: Come back to this if/when physics works
}

/*Calculate Movement vector on X  based on axis input */
void ATankPawn::GetMoveDirection(float MoveInput)
{
	MovementDirection.X = FMath::Clamp(MoveInput, -1.0f, 1.0f);
	MovementDirection.X = MovementDirection.X * GetWorld()->DeltaTimeSeconds * MoveSpeed;
	
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Functions For Movement ///////////////////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ATankPawn::GetMoveRotation(float rotationInput)
{
	float desiredRotate = rotationInput * TurnSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator Rotation = FRotator(0, desiredRotate, 0);
	PlayerRotation = FQuat(Rotation);
}

void ATankPawn::GetTurretRotation(float MouseInput)
{
	MouseFloat = MouseInput * TurretRotationSpeed * GetWorld()->DeltaTimeSeconds;
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Movement and Rotation client and server functions ////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ATankPawn::MovePlayer()
{
		if (!MovementDirection.IsNearlyZero())
	{
		if (IsLocallyControlled())
			{
				UE_LOG(LogTemp, Warning, TEXT("Current Movement Vector: %s"), *MovementDirection.ToCompactString())
				AddMovementInput(GetActorForwardVector(), 2);
				MovePlayer_Server_Implementation(MovementDirection);
			}
	}

}

/* Server Implementation for replicating movement */
void ATankPawn::MovePlayer_Server_Implementation(FVector MoveDirection)
{
	if (!IsLocallyControlled())
	{
		AddActorLocalOffset(MoveDirection, true);
	}
		
	
}


void ATankPawn::RotatePlayer(FQuat PlayerRot)
{
	if (IsLocallyControlled())
	{
		//AddActorLocalRotation(PlayerRot);
		AddActorWorldRotation(PlayerRot);
		RotatePlayer_Server(PlayerRot);
	}
}

/* Server Replication for Player rotation */
void ATankPawn::RotatePlayer_Server_Implementation(FQuat PlayerRotRep)
{
	if(!IsLocallyControlled()) AddActorWorldRotation(PlayerRotRep);
}

float ATankPawn::GetJumpBaseHeight()
{
	return GetOwner()->GetActorLocation().Z;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PAWN DESTRUCTION /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ATankPawn::DestroyPawn()
{
	Super::DestroyPawn();

	//TODO: Hide all components, stop input
}
