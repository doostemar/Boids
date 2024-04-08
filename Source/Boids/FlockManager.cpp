// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockManager.h"
#include "Boid.h"


void AFlockManager::AddBoidToFlock(ABoid* Boid)
{
	Flock.Emplace(Boid);
}

const TArray<ABoid*> AFlockManager::GetFlock()
{
	return Flock;
}

float AFlockManager::GetMaxSpeed()
{
	return MaxSpeed;
}

float AFlockManager::GetMinSpeed()
{
	return MinSpeed;
}

float AFlockManager::GetPerceptionDistance()
{
	return PerceptionDistance;
}

float AFlockManager::GetAlignmentScalar()
{
	return AlignmentScalar;
}

float AFlockManager::GetSeparationScalar()
{
	return SeparationScalar;
}

float AFlockManager::GetCohesionScalar()
{
	return CohesionScalar;
}

float AFlockManager::GetAvoidanceScalar()
{
	return AvoidanceScalar;
}

float AFlockManager::GetBlindSpotAngle()
{
	return BlindSpotAngle;
}