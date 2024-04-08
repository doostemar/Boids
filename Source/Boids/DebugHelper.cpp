// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugHelper.h"


void UDebugHelper::DrawDebugLineFromOrigin(UWorld* World, FVector End, FColor color)
{
	// Draw the debug line.
	DrawDebugLine(
		World,
		FVector::ZeroVector,
		End,
		color,
		false,
		1.f,          // lifetime
		0,             // depth priority
		2.0f           // thickness
	);
}

//---------------------------------------------------------------------------------
void UDebugHelper::DrawDebugLineFromOrigin(UWorld* World, FVector End, FColor color, float lifetime)
{
	// Draw the debug line.
	DrawDebugLine(
		GetWorld(),
		FVector::ZeroVector,
		End,
		color,
		false,
		lifetime,          // lifetime
		0,             // depth priority
		2.0f           // thickness
	);
}

//---------------------------------------------------------------------------------
void UDebugHelper::DrawDetectionDistanceDebugLine(UWorld* World, FVector ActorLocation, FVector End, FColor color)
{
	// Draw the debug line.
	DrawDebugLine(
		World,
		ActorLocation,
		End,
		color,
		false,
		0.0f,          // lifetime
		0,             // depth priority
		2.0f           // thickness
	);
}

//---------------------------------------------------------------------------------
void UDebugHelper::DrawDetectionDistanceDebugLine(UWorld* World, FVector ActorLocation, FVector End, FColor color, float lifetime)
{
	// Draw the debug line.
	DrawDebugLine(
		World,
		ActorLocation,
		End,
		color,
		false,
		lifetime,
		0,             // depth priority
		2.0f           // thickness
	);
}