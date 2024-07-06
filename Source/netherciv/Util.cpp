// Fill out your copyright notice in the Description page of Project Settings.


#include "Util.h"

Util::Util()
{
}

Util::~Util()
{
}

FVector Util::GetVectorAtDistance(FVector vector, double distance) {
	vector.Normalize();
	vector.X *= distance;
	vector.Y *= distance;
	vector.Z *= distance;
	return vector;
}

FVector Util::RotateRelativeToVectorAndQuat(FVector in, FVector rotateAround, FQuat quat) {
	return quat.RotateVector(in - rotateAround) + rotateAround;
}

void Util::LogVector(FVector in) {
	UE_LOG(LogTemp, Display, TEXT("%.2f,%.2f,%.2f"), in.X, in.Y, in.Z);
}