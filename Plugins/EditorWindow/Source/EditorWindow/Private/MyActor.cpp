// Fill out your copyright notice in the Description page of Project Settings.

#include "MyActor.h"


// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//create root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));

	RootComponent = Root;

	//create the static mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachTo(Root);

	//assign cylinder to static mesh asset
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	UStaticMesh* Asset = MeshAsset.Object;

	//assign asset to SM
	Mesh->SetStaticMesh(Asset);
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyActor::Spawn(FVector SpawnLocation, FRotator SpawnRotation, FActorSpawnParameters SpawnParams)
{
	GetWorld()->SpawnActor<AMyActor>(SpawnLocation, SpawnRotation, SpawnParams);
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

