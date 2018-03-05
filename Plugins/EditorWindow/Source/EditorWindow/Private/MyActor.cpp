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
	Mesh->SetMobility(EComponentMobility::Movable);

	//assign asset to SM
	Mesh->SetStaticMesh(Asset);

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	this->SetActorScale3D(this->scale);
}

//Works
void AMyActor::Spawn(FVector SpawnLocation, FRotator SpawnRotation)
{
	
	
	UE_LOG(LogTemp, Warning, TEXT("when spawning scale = %f %f %f"), this->GetActorScale3D().X, this->GetActorScale3D().Y, this->GetActorScale3D().Z);
	UWorld* const World = GWorld->GetWorld();
	if (World)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld ok"));
		World->SpawnActor<AMyActor>(SpawnLocation, SpawnRotation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld = %s"), World);
	}
}

//Does not work
AActor * AMyActor::SpawnWrapper(FVector SpawnLocation, FRotator SpawnRotation)
{
	UWorld* w = GEngine->GetWorld();

	if (w)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld ok"));
		w->SpawnActor<AMyActor>(SpawnLocation, SpawnRotation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetWorld = %s"), w);
	}
	return nullptr;
}

void AMyActor::SetScale(FVector scale)
{
	this->scale = scale;
	UE_LOG(LogTemp, Warning, TEXT("this->scale = %f %f %f"), this->scale.X, this->scale.Y, this->scale.Z);
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AMyActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("calling postinitialize"));
	this->SetActorScale3D(this->scale);
}