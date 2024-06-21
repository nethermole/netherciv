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
	double scale = 100.0;	//equal to flatRadiusHex
	double flatRadiusHex = scale;
	double edgeLength = 2 * (flatRadiusHex / tan(FMath::DegreesToRadians(60)));

	int centerPentagonAngle = 360 / 5;
	int centerHexagonAngle = 360 / 6;

	double flatRadiusPent = (edgeLength / 2) / tan(FMath::DegreesToRadians(centerPentagonAngle / 2));


	TArray<AHexGlobeTile*> hexTiles = {};
	TArray<APentGlobeTile*> pentTiles = {};

	//
	//	THIS IS THE ONE
	//
	int hexesBetweenPents = 100;

	double outerDihedralAngle = (180 - DIHEDRAL_ANGLE) / (hexesBetweenPents + 1);
	UE_LOG(LogTemp, Display, TEXT("outerDihedralAngle=%f"), outerDihedralAngle);

	APentGlobeTile* originPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, 0)));
	pentTiles.Add(originPent);

	//5ths of northern hemisphere
		//hexagon longitude
		TArray<AHexGlobeTile*> hexGlobeTileRow = {};

		double totalHexDx = flatRadiusPent;
		double totalHexDz = 0;
		for (int longitudeHexesPlaced = 0; longitudeHexesPlaced < hexesBetweenPents; longitudeHexesPlaced++) {
			double hexDx = cos(FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1))) * flatRadiusHex;
			double hexDz = sin(FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1))) * flatRadiusHex;

			totalHexDx += hexDx;
			totalHexDz += hexDz;

			FVector hexLocation = FVector(totalHexDx, 0, -1 * totalHexDz);
			FQuat hexRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1)));
			hexRotation = FQuat(hexRotation.GetAxisZ(), FMath::DegreesToRadians(centerHexagonAngle / 2)) * hexRotation;		//make the flats line up

			AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, hexLocation, hexRotation.Rotator());

			hexGlobeTileRow.Add(newHex);	//collect them for reuse
			hexTiles.Add(newHex);

			//finish crossing the hexagon
			totalHexDx += hexDx;
			totalHexDz += hexDz;
		}

		//row 1 pentagon
		double row1PentDx = cos(FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1))) * flatRadiusPent;
		double row1PentDz = sin(FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1))) * flatRadiusPent;

		totalHexDx += row1PentDx;
		totalHexDz += row1PentDz;

		FVector row1PentLocation = FVector(totalHexDx, 0, -1 * totalHexDz);
		FQuat row1PentRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1)));
		row1PentRotation = FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle / 2)) * row1PentRotation;		//make the flats line up

		APentGlobeTile* row1Pent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, row1PentLocation, row1PentRotation.Rotator());
		pentTiles.Add(row1Pent);

		//rotated hex row
		for (int i = 0; i < hexGlobeTileRow.Num(); i++) {
			FVector hexPosition = hexGlobeTileRow[i]->GetTransform().GetLocation();
			FQuat hexRotation = hexGlobeTileRow[i]->GetTransform().GetRotation();

			//do 2 sides
			for (int j = 1; j <= 3; j++) {
				FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(hexPosition, row1PentLocation, FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)));
				FQuat rotatedRotation = FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)) * hexRotation;

				//only fully do horizontal hexes. do HALF middle rows
				if (j == 1) {
					AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
					hexTiles.Add(newHex);
				}
				else {
					if (i <= hexGlobeTileRow.Num() / 2) {
						AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
						hexTiles.Add(newHex);
					}

				}
			}

		}

//Do 5ths rotations
		int hemisphereFifthHexTileCount = hexTiles.Num();
		for (int i = 0; i < hemisphereFifthHexTileCount; i++) {
			for (int j = 1; j < 5; j++) {
				FVector hexPosition = hexTiles[i]->GetTransform().GetLocation();
				FQuat hexRotation = hexTiles[i]->GetTransform().GetRotation();

				FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(hexPosition, FVector(0, 0, 0), FQuat(originPent->GetTransform().GetRotation().GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)));
				FQuat rotatedRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)) * hexRotation;

				AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				hexTiles.Add(newHex);
			}
		}

		int hemisphereFifthPentTileCount = pentTiles.Num();
		for (int i = 1; i < hemisphereFifthPentTileCount; i++) {	//start at 1 to skip north pole
			for (int j = 1; j < 5; j++) {
				FVector pentPosition = pentTiles[i]->GetTransform().GetLocation();
				FQuat pentRotation = pentTiles[i]->GetTransform().GetRotation();

				FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(pentPosition, FVector(0, 0, 0), FQuat(originPent->GetTransform().GetRotation().GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)));
				FQuat rotatedRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * j)) * pentRotation;

				APentGlobeTile* newPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				pentTiles.Add(newPent);
			}
		}

//Do hemisphere rotation
		double halfDistBetweenPents = (row1PentLocation - originPent->GetTransform().GetLocation()).Length() / 2;
		double sphereRadiusPent = halfDistBetweenPents / sin(FMath::DegreesToRadians(90 - (DIHEDRAL_ANGLE / 2)));

		//Raise everything by radius
		for (int i = 0; i < hexTiles.Num(); i++) {
			hexTiles[i]->SetActorLocation(hexTiles[i]->GetActorLocation() + FVector(0, 0, sphereRadiusPent));
		}
		for (int i = 0; i < pentTiles.Num(); i++) {
			pentTiles[i]->SetActorLocation(pentTiles[i]->GetActorLocation() + FVector(0, 0, sphereRadiusPent));
		}

		//Rotate over center over Y axis
		int hemisphereHexTileCount = hexTiles.Num();
		for (int i = 0; i < hemisphereHexTileCount; i++) {
				FVector hexPosition = hexTiles[i]->GetTransform().GetLocation();
				FQuat hexRotation = hexTiles[i]->GetTransform().GetRotation();

				FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(hexPosition, FVector(0, 0, 0), FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(180)));
				FQuat rotatedRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(180)) * hexRotation;

				//don't dupe tiles at height (0,0) equator
				if (!(rotatedPosition.Z < 1 && rotatedPosition.Z > -1)) {
					AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
					hexTiles.Add(newHex);
				}
		}
		int hemispherePentTileCount = pentTiles.Num();
		for (int i = 0; i < hemispherePentTileCount; i++) {
			FVector pentPosition = pentTiles[i]->GetTransform().GetLocation();
			FQuat pentRotation = pentTiles[i]->GetTransform().GetRotation();

			FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(pentPosition, FVector(0, 0, 0), FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(180)));
			FQuat rotatedRotation = FQuat(originPent->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(180)) * pentRotation;

			APentGlobeTile* newPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, rotatedPosition, rotatedRotation.Rotator());
			pentTiles.Add(newPent);

		}
}

void AGlobeMaker::DrawBoard2()
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
		APentGlobeTile* newTile = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, Util::GetVectorAtDistance(pentTiles[i]->GetActorLocation(), flatRadiusSpherePent), pentTiles[i]->GetTransform().GetRotation().Rotator());

		if (i == 0) {
			originPent = newTile;
		}
	}


// Add hexes
	int i = 0;
	//for (int i = 0; i < pentTiles.Num(); i++) {
	APentGlobeTile* pentTile = pentTiles[i];

	//Rotation
	FVector pentTileYaxis = originPent->GetTransform().GetRotation().GetAxisY();
	FQuat flipOverpentTileEdge = FQuat(pentTileYaxis, FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));


	FQuat rotateToFlat = FQuat(flipOverpentTileEdge.GetAxisZ(), FMath::DegreesToRadians((360 / 6) / 2)) * flipOverpentTileEdge;


	//Location
	UE_LOG(LogTemp, Display, TEXT("originPentZ=%f"), originPent->GetTransform().GetLocation().Z);

	UE_LOG(LogTemp, Display, TEXT("Rs+dZ=%f"), baseFlatRadiusSpherePent + (dZ/2));

	double hexdX = flatRadiusHex * cos(FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));
	double hexdZ = flatRadiusHex * sin(FMath::DegreesToRadians(90.0 - (DIHEDRAL_ANGLE / 2)));

	UE_LOG(LogTemp, Display, TEXT("hexdX=%f"), hexdX);
	UE_LOG(LogTemp, Display, TEXT("hexdZ=%f"), hexdZ);

	FVector hexLocation = originPent->GetTransform().GetLocation() + FVector(flatRadiusPent + hexdX, 0, 0) + FVector(0,0,-1 * hexdZ);

	GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, hexLocation, rotateToFlat.Rotator());

	double flatRadiusSphereHex = hexLocation.Length();


	//rotateAroundPent
	for (int j = 1; j < 5; j++) {
		FVector newLocation = Util::RotateRelativeToVectorAndQuat(hexLocation, FVector(0,0,0), FQuat(FVector::UpVector, FMath::DegreesToRadians(j * 360 / 5)));
		FRotator newRotation = (FQuat(FVector::UpVector, FMath::DegreesToRadians(j * 360 / 5)) * rotateToFlat).Rotator();

		GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, newLocation, newRotation);
	}



	UE_LOG(LogTemp, Display, TEXT("flatRadiusSpherePent=%f"), flatRadiusSpherePent);
	UE_LOG(LogTemp, Display, TEXT("flatRadiusSphereHex=%f"), flatRadiusSphereHex);
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

