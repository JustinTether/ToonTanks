// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToonTanks/ToonTanks.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ToonTanks/Attributes/TTAttributeSet.h"
#include "ToonTanks/Abilities/TTGameplayAbility.h"
#include <GameplayEffectTypes.h>
#include "TankChar.generated.h"

class UCapsuleComponent;
class AProjectileBase;
class UCameraComponent;
class UCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UPointLightComponent;
class ATTPlayerController;
class UMaterialInstanceDynamic;
class UMaterialInstance;
class UTTAbilitySystemComponent;

UCLASS()
class TOONTANKS_API ATankChar : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

protected:
	/*Override functions*/
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;



public:
	// Sets default values for this pawn's properties

	/*Ability System*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	UTTAbilitySystemComponent* AbilitySystemComponent;

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UTTAttributeSet* Attributes;

	/*Turret mesh, base mesh components*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UStaticMeshComponent* TurretMesh = nullptr; //Static mesh component for the Turret
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UStaticMeshComponent* BaseMesh = nullptr; //Static mesh component for the base of the tank

	/*Movement Components*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Mesh Movement", meta = (AllowPrivateAccess = "True"))
	UCharacterMovementComponent* BaseMovementComponent = nullptr;

	/* Replicated movement variables */
	UPROPERTY(Replicated)
	FVector MovementDirection;

	/*Textures for choosing/setting team colours*/
	UPROPERTY(EditAnywhere, Category = "Team Textures")
	UTexture* RedTeamTexture;

	UPROPERTY(EditAnywhere, Category = "Team Textures")
	UTexture* BlueTeamTexture;

	UPROPERTY(EditAnywhere, Category = "Team Textures")
	UMaterialInterface* TankMaterial;


	float MouseFloat = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	bool bCanFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "True"))
	float TurnSpeed = 100.f;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TankTeam)
	int TankTeam;

	/* Aiming */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Turret")
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = "Turret")
	float MinProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = "Turret")
	float MaxProjectileSpeed;

	UPROPERTY(EditAnywhere, Category = "Aim")
	float MinVerticalAim;

	UPROPERTY(EditAnywhere, Category = "Aim")
	float MaxVerticalAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	FVector ProjectileAimPosition;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Aim")
	float MouseVerticalOffset;

	/*Functions*/
	ATankChar();

	UFUNCTION(BlueprintCallable)
	void SetFireDelay();

	UFUNCTION(BlueprintCallable)
	void ResetFireDelay();

	UFUNCTION(NetMulticast, Reliable)
	void SetTeamColour_Server(int Team);

	UFUNCTION()
	void OnRep_TankTeam();


protected:

	/*Projectile class, and projectile spawn point component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	USceneComponent* ProjectileSpawnPoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProjectileType", meta = (AllowPrivateAccess = "True"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	/*Turret rotation, and it's server-side implementation*/
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RotateTurret_Server(float LookLocation);

	void RotateTurret(float LookAt);

	UFUNCTION()
	void AdjustProjectileSpeed(float DesiredAdjustment);

	UFUNCTION(Server, reliable)
	void AdjustProjectileSpeed_Server(float DesiredAdjustment);

	/* GameplayAbilities */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UTTGameplayAbility>> DefaultAbilities;

private:
	/*Camera component and springarm component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "True"))
	UCameraComponent* PlayerCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "True"))
	USpringArmComponent* CameraSpringArm = nullptr;

	APlayerController* TankController = nullptr;

	/* GameplayAbilities Functions */
	virtual void InitializeAttributes();
	virtual void GiveAbilities();

	UPROPERTY(Replicated)
	FRotator PlayerRotation;

	UPROPERTY(EditAnywhere)
	float TurretRotationSpeed = 300;

	void GetMoveDirection(float MoveInput); //Calculate move vector
	void GetMoveRotation(float RotationInput); //Calculate rotation Quat from axis
	void GetMouseVerticalAim(float MouseY);

	void MovePlayer();

	void GetTurretRotation(float MouseInput);

	void RotatePlayer(FRotator PlayerRot);

	void LaunchPlayer(FVector LaunchDirection);
};
