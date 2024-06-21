// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NETHERCIV_API Util
{
public:
	Util();
	~Util();

	static FVector GetVectorAtDistance(FVector vector, double distance);

	static FVector RotateRelativeToVectorAndQuat(FVector in, FVector rotateAround, FQuat quat);

	static FVector GetNormalizedCopyOfVector(FVector in);
};
