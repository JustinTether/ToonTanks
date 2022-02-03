// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "TankPawn.generated.h"

//Declare some classes we'll be using (faster than include?? I dunno...)
class USpringArmComponent;
class UCameraComponent;
class UPointLightComponent;
class ATTPlayerController;

UCLASS()
class TOONTANKS_API ATankPawn : public ATankBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess = "True"))
	UCameraComponent* PlayerCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "True"))
	USpringArmComponent* CameraSpringArm = nullptr;

	APlayerController* TankController = nullptr;
public:
	UPROPERTY(EditAnywhere, Category = "Team Textures")
	UTexture* RedTeamTexture;

	UPROPERTY(EditAnywhere, Category = "Team Textures")
	UTexture* BlueTeamTexture;

	UPROPERTY(EditAnywhere, Category="Team Textures")
	UMaterialInterface* TankMaterial;
private:
	/* Replicated movement variables */
	UPROPERTY(Replicated)
	FVector MovementDirection;
	UPROPERTY(Replicated)
	FQuat PlayerRotation;

	UPROPERTY(EditAnywhere)
	float TurretRotationSpeed = 300;

	void GetMoveDirection(float MoveInput); //Calculate move vector
	void GetMoveRotation(float RotationInput); //Calculate rotation Quat from axis

	void MovePlayer();

	void GetTurretRotation(float MouseInput);

	void RotatePlayer(FQuat PlayerRot);
	
	virtual void DestroyPawn() override;

	float PreJumpZValue = 0.f;
	
	/* Editor-facing values for maximum jump height */
	UPROPERTY(EditAnywhere)
	float JumpHeight = 300.f;

	float TargetZ = 0.f;
	float GetJumpBaseHeight();
	void Jump();
	void StopJump();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	ATankPawn();
	bool bIsJumping = false;
	
	FQuat TurretRotation;

	float MouseFloat = 0.f;

	//Some editor values for turn/movement speed as it's more feel based than anything
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "True"))
	float MoveSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "True"))
	float TurnSpeed = 100.f;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void MovePlayer_Server(FVector MoveDirection);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RotatePlayer_Server(FQuat PlayerRotRep);

	UFUNCTION(NetMulticast, Reliable)
	void SetTeamColour_Server(int Team);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;



	
};
