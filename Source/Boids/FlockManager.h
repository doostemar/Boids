// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "FlockManager.generated.h"

class ABoid;

/**
 * 
 */
UCLASS()
class BOIDS_API AFlockManager : public AInfo
{
	GENERATED_BODY()

public:
	void AddBoidToFlock(ABoid* Boid);
	const TArray<ABoid*> GetFlock();

	float GetMaxSpeed();
	float GetMinSpeed();
	float GetPerceptionDistance();
	float GetAlignmentScalar();
	float GetSeparationScalar();
	float GetCohesionScalar();
	float GetAvoidanceScalar();
	float GetBlindSpotAngle();

private:
	TArray<ABoid*> Flock;
	float MaxSpeed = 3000.f;
	float MinSpeed = 2000.f;
	float PerceptionDistance = 2000.f;
	float AlignmentScalar = 100.f;
	float SeparationScalar = 150.f;
	float CohesionScalar = 100.f;
	float AvoidanceScalar = 50;
	float BlindSpotAngle = -0.88;
};
