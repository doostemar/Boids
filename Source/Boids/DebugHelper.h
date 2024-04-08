// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebugHelper.generated.h"

/**
 * 
 */
UCLASS()
class BOIDS_API UDebugHelper : public UObject
{
	GENERATED_BODY()

public:
	void DrawDebugLineFromOrigin(UWorld* World, FVector End, FColor color);
	void DrawDebugLineFromOrigin(UWorld* World, FVector End, FColor color, float lifetime);
	void DrawDetectionDistanceDebugLine(UWorld* World, FVector ActorLocation, FVector End, FColor color);
	void DrawDetectionDistanceDebugLine(UWorld* World, FVector ActorLocation, FVector End, FColor color, float lifetime);
};
