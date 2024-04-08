// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Boid.generated.h"

class AFlockManager;
class UDebugHelper;

UCLASS()
class BOIDS_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid")
	AFlockManager* FlockManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MaxSpeed;

	FVector BoidVelocity;
	FRotator BoidRotation;

protected:

	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* BoidCore;

	UPROPERTY(EditAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BoidMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* BoidPersonalSpace;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// member variables
	int NumAvoidanceRays = 75;
	float const GOLDEN_RATIO = 1.618033988;
	TArray<FVector> AvoidanceRays;

	// avoid obstacle functions
	void ShowAvoidanceRays();
	void GenerateAvoidanceRays();
	bool IsObstructed();
	FVector AvoidObstacles();

	// main algorithm functions
	bool IsNeighbor(ABoid* Flockmate);
	TArray<ABoid*> FindNeighbors();
	FVector Align(TArray<ABoid*> Neighbors);
	FVector Cohere(TArray<ABoid*> Neighbors);
	FVector Separate(TArray<ABoid*> Neighbors);
	void Steer(float DeltaTime); // combines all

	// debugging and utility
	FVector GetPerceptionVector(FVector direction);

	UDebugHelper* DebugHelper;
};