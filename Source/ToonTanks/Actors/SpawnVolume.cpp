// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "GameFramework/PlayerStart.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision Volume"));
	SpawnBox->SetupAttachment(RootComponent);

}

TArray<AActor*> ASpawnVolume::GetAssociatedSpawnPoints()
{
	TArray<AActor*> TeamStartPoints;
	SpawnBox->GetOverlappingActors(TeamStartPoints, APlayerStart::StaticClass() );
	
	for (auto PlayerStart : TeamStartPoints)
	{
		UE_LOG(LogTemp, Error, TEXT("Start Location: %s"), *PlayerStart->GetActorLocation().ToString());
	}

	return TeamStartPoints;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Test;

	Test = GetAssociatedSpawnPoints();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

