// Fill out your copyright notice in the Description page of Project Settings.

//Project Includes
#include "TankChar.h"
#include "ToonTanks/Actors/ProjectileBase.h"
#include "ToonTanks/Controllers/TTPlayerController.h"
#include "ToonTanks/Components/TTAbilitySystemComponent.h"
#include "ToonTanks/Abilities/TTGameplayAbility.h"

//UE Includes
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
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

	AbilitySystemComponent = CreateDefaultSubobject<UTTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UTTAttributeSet>(TEXT("Ability Attributes"));

	bReplicates = true;

	TankTeam = -1;
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

void ATankChar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GiveAbilities();
}

void ATankChar::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	//GiveAbilities();

	//Bind inputs for abilities
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancel", "TTAbilityInputID", static_cast<int32>(TTAbilityInputID::Confirm), static_cast<int32>(TTAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

class UAbilitySystemComponent* ATankChar::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ATankChar::InitializeAttributes()
{
	if (IsValid(AbilitySystemComponent) && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}

		UE_LOG(LogTemp, Log, TEXT("Initialized Attributes"));
	}

}

void ATankChar::GiveAbilities()
{
	if (HasAuthority() && IsValid(AbilitySystemComponent))
	{
		for (TSubclassOf<UTTGameplayAbility>& StartingAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartingAbility, 1, static_cast<int32>(StartingAbility.GetDefaultObject()->AbilityInputID), this));
		}
	}
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
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATankChar::Fire);
	PlayerInputComponent->BindAxis("RotateTurret", this, &ATankChar::GetTurretRotation);
	PlayerInputComponent->BindAxis("ProjectilePower", this, &ATankChar::AdjustProjectileSpeed);
	PlayerInputComponent->BindAxis("VerticalAim", this, &ATankChar::GetMouseVerticalAim);

	//Bind inputs for abilities
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancel", "TTAbilityInputID", static_cast<int32>(TTAbilityInputID::Confirm), static_cast<int32>(TTAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
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
	float MovementSpeedAttribute = Attributes->MovementSpeed.GetCurrentValue();
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeedAttribute;

	MovementDirection.X = FMath::Clamp(MoveInput, -1.0f, 1.0f);
	MovementDirection.X = MovementDirection.X * GetWorld()->DeltaTimeSeconds * Attributes->MovementSpeed.GetBaseValue();
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
	float FireDelay = Attributes->FireDelay.GetCurrentValue();

	FTimerHandle ResetFireTimer;
	GetWorldTimerManager().SetTimer(ResetFireTimer, this, &ATankChar::ResetFireDelay, FireDelay);
}

void ATankChar::ResetFireDelay()
{
	bCanFire = true;
}

void ATankChar::AdjustProjectileSpeed(float DesiredAdjustment)
{
	if (DesiredAdjustment > 0.1 || DesiredAdjustment < -0.1)
	{
		AdjustProjectileSpeed_Server(DesiredAdjustment);
	}
}

void ATankChar::AdjustProjectileSpeed_Server_Implementation(float DesiredAdjustment)
{
		float AdjustedInput = (DesiredAdjustment * 100);
		float NewProjectileSpeed = FMath::Clamp(ProjectileSpeed + AdjustedInput, MinProjectileSpeed, MaxProjectileSpeed);
		UE_LOG(LogTemp, Error, TEXT("[SERVER] Current new ProjectileSpeed: %f"), NewProjectileSpeed);

		//Create runtime GE to apply ProjectileSpeed override
		UGameplayEffect* ProjectileSpeedGE = NewObject<UGameplayEffect>(GetTransientPackage(), TEXT("Projectile Speed Override"));
		ProjectileSpeedGE->DurationPolicy = EGameplayEffectDurationType::Instant;
		ProjectileSpeedGE->Modifiers.SetNum(1);

		FGameplayModifierInfo& ModifierInfo = ProjectileSpeedGE->Modifiers[0];
		ModifierInfo.ModifierMagnitude = FScalableFloat(AdjustedInput);
		ModifierInfo.ModifierOp = EGameplayModOp::Additive;
		ModifierInfo.Attribute = Attributes->GetProjectileSpeedAttribute();

		AbilitySystemComponent->ApplyGameplayEffectToSelf(ProjectileSpeedGE, 1.0, AbilitySystemComponent->MakeEffectContext());
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

void ATankChar::OnRep_TankTeam()
{
	SetTeamColour_Server(TankTeam);
}
