// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "FlockManager.h"
#include "DebugHelper.h"

//---------------------------------------------------------------------------------
// Sets default values
ABoid::ABoid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoidCore = CreateDefaultSubobject <UCapsuleComponent>(TEXT("Core"));
	RootComponent = BoidCore;
	BoidCore->SetCollisionObjectType(ECC_PhysicsBody);
	BoidCore->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoidCore->SetCollisionResponseToAllChannels(ECR_Overlap);

	BoidMesh = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("Boid Mesh"));
	BoidMesh->SetupAttachment(RootComponent);
	BoidMesh->SetCollisionObjectType(ECC_PhysicsBody);
	BoidMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

}

//---------------------------------------------------------------------------------
// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();

	if (FlockManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No FlockManager!"));
		Destroy();
	}

	FlockManager->AddBoidToFlock(this);
	GenerateAvoidanceRays();
	BoidRotation = FRotator(FMath::RandRange(0, 360), FMath::RandRange(0, 360), FMath::RandRange(0, 360));
	BoidVelocity = GetActorForwardVector() * FlockManager->GetMinSpeed();
}

//---------------------------------------------------------------------------------
void ABoid::GenerateAvoidanceRays()
{
	AvoidanceRays.Empty();

	float phi;
	float theta;

	for (int i = 0; i < NumAvoidanceRays; i++)
	{
		phi = FMath::Acos(1 - (2 * float(i) / NumAvoidanceRays));
		theta = 2 * UKismetMathLibrary::GetPI() * GOLDEN_RATIO * i;

		FVector NewRay = FVector(
			FMath::Cos(phi), // x (forward)
			FMath::Cos(theta) * FMath::Sin(phi),  // y (right)
			FMath::Sin(theta) * FMath::Sin(phi)); // z (up)

		// construct new ray at the end of the array
		AvoidanceRays.Add(NewRay);
	}
}

//---------------------------------------------------------------------------------
bool ABoid::IsObstructed()
{
	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), GetPerceptionVector(GetActorForwardVector()), ECC_GameTraceChannel1)) {
		FVector hitDirection = GetActorLocation() + GetPerceptionVector(GetActorForwardVector());
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------------
FVector ABoid::AvoidObstacles()
{
	FVector newDirection = GetActorForwardVector();
	float furthestObstacle = 0;

	// get the current rotation of the boid to rotate the rays
	FQuat BoidQuat = GetActorQuat();

	// cast rays to find possible directions
	for (const FVector& LocalRayDirection : AvoidanceRays) {
		// rotate the avoidance ray by the boid's current rotation
		FVector WorldSpaceRayDirection = BoidQuat.RotateVector(LocalRayDirection);
		FVector RayEndPoint = GetActorLocation() + WorldSpaceRayDirection * FlockManager->GetPerceptionDistance();
		FHitResult hitResult;
		if (GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), RayEndPoint, ECC_GameTraceChannel1))
		{
			// if there is a hit, check if it's the furthest one yet
			if (hitResult.Distance > furthestObstacle) {
				newDirection = RayEndPoint;
				furthestObstacle = hitResult.Distance;
			}
		}
		else
		{
			// if no obstacle has been found yet, then this direction is viable
			// UE_LOG(LogTemp, Warning, TEXT("%s found a CLEAR path"), *this->GetActorLabel());
			return RayEndPoint;
		}
	}

	// UE_LOG(LogTemp, Display, TEXT("%s found an UNCLEAR path"), *this->GetActorLabel());
	return newDirection;
}

TArray<ABoid*> ABoid::FindNeighbors()
{
	TArray<ABoid*> Neighbors;
	TArray<ABoid*> Flock = FlockManager->GetFlock();
	for (ABoid* Flockmate : Flock)
	{
		if (Flockmate != nullptr && Flockmate != this && FVector::Dist(Flockmate->GetActorLocation(), this->GetActorLocation()) < FlockManager->GetPerceptionDistance()
			&& FVector::DotProduct((Flockmate->GetActorLocation() - this->GetActorLocation()).GetSafeNormal(), this->GetActorForwardVector()) >= FlockManager->GetBlindSpotAngle())
		{
			Neighbors.Emplace(Flockmate);
		}
	}
	return Neighbors;
}

FVector ABoid::Align(TArray<ABoid*> Neighbors)
{
	FVector AverageVelocity = FVector::ZeroVector;
	int TotalNeighbors = Neighbors.Num();

	for (ABoid* Neighbor : Neighbors)
	{
		AverageVelocity += Neighbor->BoidVelocity;
	}

	if (TotalNeighbors)
	{
		AverageVelocity /= TotalNeighbors;
	}
	
	FVector AlignmentForce = AverageVelocity - BoidVelocity;
	AlignmentForce = AlignmentForce.GetSafeNormal() * FlockManager->GetAlignmentScalar();

	return AlignmentForce;
}

FVector ABoid::Cohere(TArray<ABoid*> Neighbors)
{
	FVector AverageLocation = FVector::ZeroVector;
	int TotalNeighbors = Neighbors.Num();

	for (ABoid* Neighbor : Neighbors)
	{
		AverageLocation += Neighbor->GetActorLocation();
	}

	if (TotalNeighbors)
	{
		AverageLocation /= TotalNeighbors;
	}

	FVector CohesionForce = AverageLocation - GetActorLocation();
	CohesionForce = CohesionForce.GetSafeNormal() * FlockManager->GetCohesionScalar();

	return CohesionForce;
}

FVector ABoid::Separate(TArray<ABoid*> Neighbors)
{
	FVector SeparationForce = FVector::ZeroVector;
	int TotalNeighbors = Neighbors.Num();

	for (ABoid* Neighbor : Neighbors)
	{
		FVector AwayVector = GetActorLocation() - Neighbor->GetActorLocation();
		float Distance = FVector::Dist(this->GetActorLocation(), Neighbor->GetActorLocation());
		AwayVector /= Distance;
		SeparationForce += AwayVector;
	}

	// Average the separation force based on the number of nearby boids
	if (TotalNeighbors)
	{
		SeparationForce /= TotalNeighbors;
	}
	SeparationForce = SeparationForce.GetSafeNormal() * FlockManager->GetSeparationScalar();

	return SeparationForce;
}

void ABoid::Steer(float DeltaTime)
{
	FVector Acceleration = FVector::ZeroVector;

	SetActorLocation(this->GetActorLocation() + (BoidVelocity * DeltaTime));
	BoidRotation = FMath::RInterpTo(BoidRotation, BoidVelocity.Rotation(), DeltaTime, 5.f);
	SetActorRotation(BoidRotation);

	if (IsObstructed())
	{
		FVector AvoidanceVector = AvoidObstacles();
		Acceleration = AvoidanceVector - GetActorLocation();
	}
	else
	{
		TArray<ABoid*> Neighbors = FindNeighbors();
		Acceleration += Align(Neighbors);
		Acceleration += Cohere(Neighbors);
		Acceleration += Separate(Neighbors);
	}

	BoidVelocity += Acceleration;
	BoidVelocity = BoidVelocity.GetClampedToSize(FlockManager->GetMinSpeed(), FlockManager->GetMaxSpeed());
}

//---------------------------------------------------------------------------------
// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// ShowAvoidanceRays();
	Steer(DeltaTime);
}

//---------------------------------------------------------------------------------
void ABoid::ShowAvoidanceRays()
{
	if (DebugHelper == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DebugHelper has not been attached"));
		return;
	}

	// Get the current rotation of the boid as a quaternion
	FQuat BoidQuat = GetActorQuat();

	bool firstPass = false;

	for (const FVector& LocalRayDirection : AvoidanceRays)
	{
		// Rotate the local ray direction by the boid's current rotation to get the world-space direction
		FVector WorldSpaceRayDirection = BoidQuat.RotateVector(LocalRayDirection);

		// Calculate the world-space end point for the ray
		FVector RayEndPoint = GetActorLocation() + WorldSpaceRayDirection * FlockManager->GetPerceptionDistance();

		if (firstPass == false)
		{
			firstPass = true;
			DebugHelper->DrawDetectionDistanceDebugLine(GetWorld(), GetActorLocation(), RayEndPoint, FColor::White);
		}
		DebugHelper->DrawDetectionDistanceDebugLine(GetWorld(), GetActorLocation(), RayEndPoint, FColor::Cyan);
	}
}

//---------------------------------------------------------------------------------
FVector ABoid::GetPerceptionVector(FVector direction)
{
	return GetActorLocation() + direction * FlockManager->GetPerceptionDistance();
}