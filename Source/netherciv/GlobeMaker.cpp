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

