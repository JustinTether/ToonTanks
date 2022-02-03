// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoostPad.generated.h"

class UBoxComponent;

UCLASS()
class TOONTANKS_API ABoostPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostPad();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Collision")
	UBoxComponent* BoxCollison;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mesh")
	UStaticMeshComponent* PowerupMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSwept, const FHitResult& SweepResult);
};
