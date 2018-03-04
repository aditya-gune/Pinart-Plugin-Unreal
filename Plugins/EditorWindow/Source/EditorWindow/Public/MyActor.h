// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Components/StaticMeshComponent.h"
#include "MyActor.generated.h"

UCLASS()
class EDITORWINDOW_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		USceneComponent *Root;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent *Mesh;

	float x, y, z;		//xyz of the pin

	/*
	* generateXY gets the pixel position and returns the XY of the cylinder
	* generateZ gets the pixel rgb and returns the height of the cylinder
	*/
	float generateXY(float pos);
	float generateZ(float value);

	FVector location;	//holds location of the pin
	FVector oldLoc;		//holds location of previous pin
	int counter;		//holds counter of pins so we know when to stop
	bool overlap = false;
	
	void Spawn(FVector SpawnLocation, FRotator SpawnRotation, FActorSpawnParameters SpawnParams);

};
