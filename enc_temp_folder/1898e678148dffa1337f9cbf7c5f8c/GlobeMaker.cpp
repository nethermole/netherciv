// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobeMaker.h"
#include "Util.h"

#include "math.h"


#define PI 3.14159265
#define DIHEDRAL_ANGLE 116.56505

// Sets default values
AGlobeMaker::AGlobeMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}



void AGlobeMaker::DrawBoard()
{
	//Strategy:
	//Spawn NorthPole pent.Flat side forward;
	//	Create adjacentForward tile
	//		Calculate the rotation
	//		Calculate the pos
	//	Do prior step in each of 5 cardinal directions

	double scale = 100.0;	//equal to flatRadiusHex
	double flatRadiusHex = scale;
	double edgeLength = 2 * (flatRadiusHex / tan(FMath::DegreesToRadians(60)));

	int centerHexAngle = 360 / 6;	//60
	double halfHexCornerAngle = 180 - 90 - (centerHexAngle / 2);	//also 60
	double radiusHex = flatRadiusHex / sin(FMath::DegreesToRadians(halfHexCornerAngle));

	int centerPentAngle = 360 / 5;					//72
	int halfCenterPentAngle = centerPentAngle / 2;	//36
	double flatRadiusPent = (edgeLength / 2) / tan(FMath::DegreesToRadians(halfCenterPentAngle));
	double radiusPent = (edgeLength / 2) / sin(FMath::DegreesToRadians(halfCenterPentAngle));

	double baseFlatRadiusSpherePent = tan(FMath::DegreesToRadians(DIHEDRAL_ANGLE / 2)) * flatRadiusPent;


	UE_LOG(LogTemp, Display, TEXT("baseFlatRadiusSpherePent=%f"), baseFlatRadiusSpherePent);
	//UE_LOG(LogTemp, Display, TEXT("flatRadiusHex=%f"), flatRadiusHex);
	//UE_LOG(LogTemp, Display, TEXT("radiusHex=%f"), radiusHex);
	//UE_LOG(LogTemp, Display, TEXT("edgeLength=%f"), edgeLength);
	//UE_LOG(LogTemp, Display, TEXT("flatRadiusPent=%f"), flatRadiusPent);
	//UE_LOG(LogTemp, Display, TEXT("radiusPent=%f"), radiusPent);


	TArray<APentGlobeTile*> pentTiles = {};

	int numSides = 5;	//because originPent

//North Pole
	APentGlobeTile* originPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, baseFlatRadiusSpherePent)));
	pentTiles.Add(originPent);

//Row 1
	//Rotation
	FVector pentYaxis = originPent->GetTransform().GetRotation().GetAxisY();
	FQuat flipOverEdge = FQuat(pentYaxis, FMath::DegreesToRadians(360.0-DIHEDRAL_ANGLE));

	FVector pentXaxis = originPent->GetTransform().GetRotation().GetAxisX();
	FQuat flipOverPent = FQuat(pentXaxis, FMath::DegreesToRadians(180));

	FQuat rotation = originPent->GetActorRotation().Quaternion() * flipOverEdge * flipOverPent;


	//Position
	FVector position = originPent->GetTransform().GetLocation();
	double angleBetweenPents = (180 - 90 - (DIHEDRAL_ANGLE / 2)) * 2;	//64ish
	UE_LOG(LogTemp, Display, TEXT("angleBetweenPents=%f"), angleBetweenPents);
	UE_LOG(LogTemp, Display, TEXT("position.Z=%f"), position.Z);
	UE_LOG(LogTemp, Display, TEXT("position.X=%f"), position.X);

	FVector newPosition = Util::RotateRelativeToVectorAndQuat(position, FVector(0, 0, 0), FQuat(pentYaxis, FMath::DegreesToRadians(angleBetweenPents)));

	//Radial
	for (int i = 0; i < 5; i++) {
		FQuat radialRot(FVector::UpVector, FMath::DegreesToRadians(i * 360 / numSides));

		FVector radialPosition = Util::RotateRelativeToVectorAndQuat(newPosition, position, radialRot);
		FQuat radialRotation = rotation * radialRot;

		//Placement
		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, radialPosition + FVector(0, 0, baseFlatRadiusSpherePent), (radialRot * rotation).Rotator()));
	}


//Flip them
	int upperPentCount = pentTiles.Num();
	for (int i = 0; i < upperPentCount; i++) {
		APentGlobeTile* pentTile = pentTiles[i];

		FQuat flipOverPentTileY = FQuat(pentTile->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(180));

		FVector invertedPosition = Util::RotateRelativeToVectorAndQuat(pentTile->GetActorLocation(), FVector(0, 0, 0), flipOverPentTileY);
		FRotator invertedRotation = (pentTile->GetTransform().GetRotation() * flipOverPentTileY).Rotator();
		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, invertedPosition + FVector(0, 0, 0), invertedRotation));
	}

//Inflate pentagon locations (keep old for now)
	//increase sphere radius. Calculate dZ of the adjacent hexes from origin, add to current distance from center
	float dZ = flatRadiusHex / cos(FMath::DegreesToRadians(90.0 - (90 - (DIHEDRAL_ANGLE / 2))));
	float flatRadiusSpherePent = baseFlatRadiusSpherePent + dZ;

	int pentCount = pentTiles.Num();
	for (int i = 0; i < pentCount; i++) {
		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, Util::GetVectorAtDistance(pentTiles[i]->GetActorLocation(), flatRadiusSpherePent), pentTiles[i]->GetTransform().GetRotation().Rotator()));
		//pentTiles[i]->SetActorLocation(Util::GetVectorAtDistance(pentTiles[i]->GetActorLocation() - baseSphereCenter, flatRadiusSpherePent));
	}


// Add hexes
	int i = 0;
	//for (int i = 0; i < pentTiles.Num(); i++) {
	APentGlobeTile* pentTile = pentTiles[i];

	//Rotation
	FVector pentTileYaxis = originPent->GetTransform().GetRotation().GetAxisY();
	FQuat flipOverpentTileEdge = FQuat(pentTileYaxis, FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));


	//Location
	UE_LOG(LogTemp, Display, TEXT("originPentZ=%f"), originPent->GetTransform().GetLocation().Z);

	UE_LOG(LogTemp, Display, TEXT("Rs+dZ=%f"), baseFlatRadiusSpherePent + (dZ/2));

	double hexdX = flatRadiusHex * cos(FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));
	double hesdZ = flatRadiusHex * sin(FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));

	UE_LOG(LogTemp, Display, TEXT("hexdX=%f"), hexdX);
	UE_LOG(LogTemp, Display, TEXT("hesdZ=%f"), hesdZ);

	//GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, hexLocation, flipOverpentTileEdge.Rotator());



	UE_LOG(LogTemp, Display, TEXT("flatRadiusSpherePent=%f"), flatRadiusSpherePent);
	//UE_LOG(LogTemp, Display, TEXT("flatRadiusSphereHex=%f"), flatRadiusSphereHex);
	UE_LOG(LogTemp, Display, TEXT("Board drawn with %d tiles"), pentTiles.Num());
}

// Called when the game starts or when spawned
void AGlobeMaker::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void AGlobeMaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

