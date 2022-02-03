// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankBase.generated.h"

class UCapsuleComponent;
class AProjectileBase;
class UCameraComponent;
class UPawnMovementComponent;

UCLASS()
class TOONTANKS_API ATankBase : public APawn
{
	GENERATED_BODY()

protected: 

	void Fire();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Fire_Server();

	virtual void DestroyPawn();


	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RotateTurret_Server(float LookLocation);

	void RotateTurret(float LookAt);

public:
	//This is in protected so that the pawn can attach a camera to the turret so that the camera follows mouse position
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UStaticMeshComponent* TurretMesh = nullptr; //Static mesh component for the Turret

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UStaticMeshComponent* BaseMesh = nullptr; //Static mesh component for the base of the tank


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Mesh Movement", meta = (AllowPrivateAccess = "True"))
	UPawnMovementComponent* BaseMovementComponent = nullptr;

private: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	UCapsuleComponent* CapsuleComponent = nullptr; //Collision component

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "True"))
	USceneComponent* ProjectileSpawnPoint = nullptr; //Projectile spawn point???

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="ProjectileType", meta=(AllowPrivateAccess = "True"))
	TSubclassOf<AProjectileBase> ProjectileClass;


public:
	// Sets default values for this pawn's properties
	ATankBase();
};
