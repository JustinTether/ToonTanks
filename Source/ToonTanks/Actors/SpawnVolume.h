// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class APlayerStart;
class UBoxComponent;

UCLASS()
class TOONTANKS_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();
	TArray<AActor*> GetAssociatedSpawnPoints();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TeamID = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UBoxComponent* SpawnBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
