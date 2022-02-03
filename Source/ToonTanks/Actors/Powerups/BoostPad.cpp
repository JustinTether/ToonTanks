// Fill out your copyright notice in the Description page of Project Settings.


#include "BoostPad.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "../../Pawns/TankChar.h"

// Sets default values
ABoostPad::ABoostPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxCollison = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Component"));
	BoxCollison->SetBoxExtent(FVector(75, 75, 50));
	RootComponent = BoxCollison;

	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	PowerupMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABoostPad::BeginPlay()
{
	Super::BeginPlay();
	BoxCollison->OnComponentBeginOverlap.AddDynamic(this, &ABoostPad::OnOverlap);
}

void ABoostPad::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSwept, const FHitResult& SweepResult)
{
	if (ATankChar* Tank = Cast<ATankChar>(OtherActor))
	{
		FVector LaunchDirection = FVector(Tank->MovementDirection.X, 0, 500);
		Tank->LaunchCharacter(LaunchDirection, false, false);
	}

}

