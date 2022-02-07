// Fill out your copyright notice in the Description page of Project Settings.


#include "TankChar.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Actors/ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "../Controllers/TTPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
ATankChar::ATankChar()
{
	//Create the needed components of our tank, the base, the turret and the spawn point for your bullet
	PrimaryActorTick.bCanEverTick = true;

	//Create the base mesh component which will hold the Tank Mesh
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tank Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);
	BaseMesh->SetIsReplicated(true);
	//BaseMesh->SetSimulatePhysics(true);

	//Create the movement component
	//BaseMovementComponent = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("Tank MovementComponent"));

	//Create the turret mesh component, attach it to the base mesh (So they follow eachother)
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);
	TurretMesh->SetIsReplicated(true);

	//Projectile spawn point, self explanatory
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Tank Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	//Camera and springarm creation
	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Spring Arm"));
	CameraSpringArm->SetupAttachment(TurretMesh);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Controlled Camera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);

	bReplicates = true;

	TankTeam = -1;
	TurretFireDelay = 0.5f;
	MinProjectileSpeed = 400.f;
	MaxProjectileSpeed = 5000.f;
	ProjectileSpeed = MinProjectileSpeed;
	MinVerticalAim = -20.0f;
	MaxVerticalAim = 20.0f;
	MouseVerticalOffset = 0;

	//SetRole(ROLE_Authority);
}

void ATankChar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	DOREPLIFETIME(ATankChar, MovementDirection);
	DOREPLIFETIME(ATankChar, PlayerRotation);
	DOREPLIFETIME(ATankChar, TankTeam);
	DOREPLIFETIME(ATankChar, ProjectileSpeed);
	DOREPLIFETIME(ATankChar, MouseVerticalOffset);
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ATankChar::BeginPlay()
{
	Super::BeginPlay();
	TankController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	FAttachmentTransformRules newAttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	//ProjectileSpawnPoint->AttachToComponent(TurretMesh, newAttachRules, FName(TEXT("ProjectileSpawnSocket")));
}

// Called every frame
void ATankChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotatePlayer(PlayerRotation);
	MovePlayer();

	if (TankController)
	{
		if (MouseFloat > 0.001 || MouseFloat < -0.001) RotateTurret(MouseFloat);


	}
}

void ATankChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATankChar::GetMoveDirection);
	PlayerInputComponent->BindAxis("Turn", this, &ATankChar::GetMoveRotation);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATankChar::Fire);
	PlayerInputComponent->BindAxis("RotateTurret", this, &ATankChar::GetTurretRotation);
	PlayerInputComponent->BindAxis("ProjectilePower", this, &ATankChar::AdjustProjectileSpeed);
	PlayerInputComponent->BindAxis("VerticalAim", this, &ATankChar::GetMouseVerticalAim);
}

void ATankChar::RotateTurret(float LookAt)
{
	if (IsLocallyControlled())
	{
		FRotator TurretRotation = FRotator{ 0,0,0 };
		TurretRotation.Yaw = LookAt;
		TurretMesh->AddRelativeRotation(TurretRotation);
		RotateTurret_Server(LookAt);
		
	}
}

void ATankChar::GetMouseVerticalAim(float MouseY)
{
	MouseVerticalOffset += MouseY * TurnSpeed * GetWorld()->DeltaTimeSeconds;
	MouseVerticalOffset = FMath::Clamp(MouseVerticalOffset, MinVerticalAim, MaxVerticalAim);
}

void ATankChar::GetMoveDirection(float MoveInput)
{
	MovementDirection.X = FMath::Clamp(MoveInput, -1.0f, 1.0f);
	MovementDirection.X = MovementDirection.X * GetWorld()->DeltaTimeSeconds * MoveSpeed;
}

void ATankChar::GetMoveRotation(float RotationInput)
{
	float desiredRotate = RotationInput * TurnSpeed * GetWorld()->DeltaTimeSeconds;
	FRotator Rotation = FRotator(0, desiredRotate, 0);
	PlayerRotation = Rotation;
}

void ATankChar::GetTurretRotation(float MouseInput)
{
	MouseFloat = MouseInput * TurretRotationSpeed * GetWorld()->DeltaTimeSeconds;
}

void ATankChar::MovePlayer()
{
	if (!MovementDirection.IsNearlyZero())
	{
		if (IsLocallyControlled())
		{
			AddMovementInput(GetActorForwardVector(), MovementDirection.X);
		}
	}
}

void ATankChar::RotatePlayer(FRotator PlayerRot)
{
	if (IsLocallyControlled() && !PlayerRot.IsNearlyZero())
	{
		FRotator CurrentRotation = GetController()->GetControlRotation();
		CurrentRotation += PlayerRot;
		GetController()->SetControlRotation(CurrentRotation);
	}
}

void ATankChar::LaunchPlayer(FVector LaunchDirection)
{
	//TODO: LaunchPawn has become depreceated, replace this with another similar function
	//LaunchPawn(MovementDirection, false, false); //Call this function when boosting the player by some amount, maybe using jump pads?
}

void ATankChar::RotateTurret_Server_Implementation(float LookLocation)
{
	if (!IsLocallyControlled())
	{
		FRotator TurretRotation = FRotator{ 0,0,0 };
		TurretRotation.Yaw = LookLocation;
		TurretMesh->AddRelativeRotation(TurretRotation);
	}
}

void ATankChar::SetFireDelay()
{
	bCanFire = false;
	FTimerHandle ResetFireTimer;
	GetWorldTimerManager().SetTimer(ResetFireTimer, this, &ATankChar::ResetFireDelay, TurretFireDelay);
}

void ATankChar::ResetFireDelay()
{
	bCanFire = true;
}

void ATankChar::AdjustProjectileSpeed(float DesiredAdjustment)
{
	if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		float AdjustedInput = (DesiredAdjustment * 100);
		ProjectileSpeed = FMath::Clamp(ProjectileSpeed + AdjustedInput, MinProjectileSpeed, MaxProjectileSpeed);
		AdjustProjectileSpeed_Server(DesiredAdjustment);
	}

}

void ATankChar::AdjustProjectileSpeed_Server_Implementation(float DesiredAdjustment)
{
	if (!IsLocallyControlled())
	{
		float AdjustedInput = (DesiredAdjustment * 100);
		ProjectileSpeed = FMath::Clamp(ProjectileSpeed + AdjustedInput, MinProjectileSpeed, MaxProjectileSpeed);
	}
}

void ATankChar::Fire()
{
	if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		//Spawn a projectile at ProjectileSpawnPoint location and Rotation with momentum towards Rotation Vector
		if (ProjectileClass && bCanFire)
		{
			//Create a new rot with altered Y? to see if the bullet fires higher
			FRotator ProjectileRotator = ProjectileSpawnPoint->GetComponentRotation();
			ProjectileRotator.Pitch += MouseVerticalOffset;

			FTransform ProjectileSpawnTransform(ProjectileRotator, ProjectileSpawnPoint->GetComponentLocation());
			auto CustomProjectile = Cast<AProjectileBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, ProjectileSpawnTransform));
			if (!IsValid(CustomProjectile))
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to spawn deferred actor"));
				return;
			}
			CustomProjectile->FindComponentByClass<UProjectileMovementComponent>()->InitialSpeed = ProjectileSpeed;

			//@TODO Please replace this hardcoded vector with one that has it's Y element changed
			CustomProjectile->FindComponentByClass<UProjectileMovementComponent>()->SetVelocityInLocalSpace(FVector(1, 0, 0));
			UGameplayStatics::FinishSpawningActor(CustomProjectile, ProjectileSpawnTransform);
			CustomProjectile->SetOwner(this);
			Fire_Server();
			SetFireDelay();
		}//@TODO 
	}
}

void ATankChar::Fire_Server_Implementation()
{
	if (!IsLocallyControlled())
	{
		//Spawn a projectile at ProjectileSpawnPoint location and Rotation with momentum towards Rotation Vector
		if (ProjectileClass)
		{
			FRotator ProjectileRotator = ProjectileSpawnPoint->GetComponentRotation();
			ProjectileRotator.Pitch += MouseVerticalOffset;

			FTransform ProjectileSpawnTransform(ProjectileSpawnPoint->GetComponentRotation(), ProjectileSpawnPoint->GetComponentLocation());
			auto CustomProjectile = Cast<AProjectileBase>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileClass, ProjectileSpawnTransform));
			if (!IsValid(CustomProjectile))
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to spawn deferred actor"));
				return;
			}
			CustomProjectile->FindComponentByClass<UProjectileMovementComponent>()->InitialSpeed = ProjectileSpeed;
			CustomProjectile->FindComponentByClass<UProjectileMovementComponent>()->SetVelocityInLocalSpace(FVector(1, 0, 0));
			UGameplayStatics::FinishSpawningActor(CustomProjectile, ProjectileSpawnTransform);
			CustomProjectile->SetOwner(this);
		}
	}
}

/* Multicast function for setting dynamic material based on TeamID */
void ATankChar::SetTeamColour_Server_Implementation(int Team)
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

void ATankChar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
}

void ATankChar::OnRep_TankTeam()
{
	SetTeamColour_Server(TankTeam);
}
