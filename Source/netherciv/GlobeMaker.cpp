// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobeMaker.h"

#include "Util.h"
#include "math.h"


#define DIHEDRAL_ANGLE 116.56505
#define SCALE 100

// Sets default values
AGlobeMaker::AGlobeMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AGlobeMaker::IsPent(AActor* actor) {
	return actor->IsA(APentGlobeTile::StaticClass());
}

bool AGlobeMaker::IsHex(AActor* actor) {
	return actor->IsA(AHexGlobeTile::StaticClass());
}

void AGlobeMaker::CalculateAttributes(int hexesBetweenPents) {
	centerPentagonAngle = 360 / 5;
	centerHexagonAngle = 360 / 6;

	outerDihedralAngle = (180 - DIHEDRAL_ANGLE) / (hexesBetweenPents + 1);

	flatRadiusHex = SCALE;
	edgeLength = 2 * (flatRadiusHex / tan(FMath::DegreesToRadians(60)));
	flatRadiusPent = (edgeLength / 2) / tan(FMath::DegreesToRadians(centerPentagonAngle / 2));
}

void AGlobeMaker::DrawBoard(double sphereX, double sphereY, double sphereZ, int hexesBetweenPents) {
	CalculateAttributes(hexesBetweenPents);
	
	TArray<AActor*> allTiles = {};

	APentGlobeTile* northPoleTile = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, 0)));
	allTiles.Add(northPoleTile);

	double longitudeHexesDx = 0;
	double longitudeHexesDz = 0;
	TArray<AHexGlobeTile*> longitudeHexes = CreateInitialLongitudeHexes(northPoleTile, hexesBetweenPents, longitudeHexesDx, longitudeHexesDz);
	allTiles.Append(longitudeHexes);

	APentGlobeTile* row1PentTile = CreateRow1Pent(longitudeHexesDx, longitudeHexesDz, hexesBetweenPents);
	allTiles.Add(row1PentTile);

	TArray<AHexGlobeTile*> row1Hexes = RotateInitialLongitudeHexesAroundRow1Pent(longitudeHexes, row1PentTile);
	allTiles.Append(row1Hexes);
	TArray<AHexGlobeTile*> equatorHexes = RotateInitialLongitudeHexesAroundRow1PentToEquator(longitudeHexes, row1PentTile);
	allTiles.Append(equatorHexes);

	TArray<AActor*> northernHemisphereTilesToRotate = {};
	northernHemisphereTilesToRotate.Append(longitudeHexes);
	northernHemisphereTilesToRotate.Add(row1PentTile);
	northernHemisphereTilesToRotate.Append(row1Hexes);
	northernHemisphereTilesToRotate.Append(equatorHexes);
	TArray<AActor*> rotatedNorthernHemisphereTiles = RotateNorthernHemisphereFifth(northernHemisphereTilesToRotate);
	allTiles.Append(rotatedNorthernHemisphereTiles);

	double halfDistBetweenPents = row1PentTile->GetActorLocation().Length() / 2;	//since origin is at 0,0
	double sphereRadiusPent = halfDistBetweenPents / sin(FMath::DegreesToRadians(90 - (DIHEDRAL_ANGLE / 2)));
	for (int i = 0; i < allTiles.Num(); i++) {
		allTiles[i]->SetActorLocation(allTiles[i]->GetActorLocation() + FVector(0,0, sphereRadiusPent));
	}

	TArray<AActor*> southernHemisphereTiles = RotateToSouthHemisphere(allTiles);
	allTiles.Append(southernHemisphereTiles);
}

TArray<AActor*> AGlobeMaker::RotateToSouthHemisphere(TArray<AActor*> northTiles) {
	TArray<AActor*> tiles = {};

	for (int i = 0; i < northTiles.Num(); i++) {
		FVector position = northTiles[i]->GetTransform().GetLocation();
		FQuat rotation = northTiles[i]->GetTransform().GetRotation();

		FQuat pivot = FQuat(FVector::YAxisVector, FMath::DegreesToRadians(180));

		FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(position, FVector(0, 0, 0), pivot);
		FQuat rotatedRotation = pivot * rotation;
		if (!(rotatedPosition.Z < 1 && rotatedPosition.Z > -1)) {	//dont dupe equator z=0 tiles
			if (IsHex(northTiles[i])) {
				AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				tiles.Add(newHex);
			}
			else if(northTiles[i]){
				APentGlobeTile* newPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				tiles.Add(newPent);
			}
			else {
				UE_LOG(LogTemp, Display, TEXT("RotateToSouthHemisphere() Unexpected tile type found during world generation"));
			}
		}
	}
	return tiles;
}

TArray<AActor*> AGlobeMaker::RotateNorthernHemisphereFifth(TArray<AActor*> northernHemisphereTilesToRotate) {
	TArray<AActor*> tiles = {};

	for (int i = 0; i < northernHemisphereTilesToRotate.Num(); i++) {
		for (int pentRotation = 1; pentRotation < 5; pentRotation++) {
			FVector position = northernHemisphereTilesToRotate[i]->GetTransform().GetLocation();
			FQuat rotation = northernHemisphereTilesToRotate[i]->GetTransform().GetRotation();

			FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(position, FVector(0, 0, 0), FQuat(FVector::UpVector, FMath::DegreesToRadians(centerPentagonAngle * pentRotation)));
			FQuat rotatedRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(centerPentagonAngle * pentRotation)) * rotation;

			if (IsHex(northernHemisphereTilesToRotate[i])) {
				AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				tiles.Add(newHex);
			}
			else {
				APentGlobeTile* newPent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, rotatedPosition, rotatedRotation.Rotator());
				tiles.Add(newPent);
			}
		}
	}

	return tiles;
}

TArray<AHexGlobeTile*> AGlobeMaker::RotateInitialLongitudeHexesAroundRow1PentToEquator(TArray<AHexGlobeTile*> longitudeHexes, APentGlobeTile* row1PentTile) {
	TArray<AHexGlobeTile*> tiles = {};

	FVector row1PentLocation = row1PentTile->GetActorLocation();
	FQuat row1PentRotation = row1PentTile->GetActorTransform().GetRotation();

	for (int pentagonAngleRotations = 2; pentagonAngleRotations <= 3; pentagonAngleRotations++) {
		for (int i = longitudeHexes.Num()-1; i >= longitudeHexes.Num() / 2; i--) {						//TODO: THIS IS A HACK AF TO GET ONLY THE FIRST HALF OF EQUATOR TILES
			FVector hexPosition = longitudeHexes[i]->GetTransform().GetLocation();
			FQuat hexRotation = longitudeHexes[i]->GetTransform().GetRotation();

			FQuat pivot = FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle * pentagonAngleRotations));

			FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(hexPosition, row1PentLocation, pivot);
			FQuat rotatedRotation = pivot * hexRotation;

			AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
			tiles.Add(newHex);
		}
	}

	return tiles;
}

TArray<AHexGlobeTile*> AGlobeMaker::RotateInitialLongitudeHexesAroundRow1Pent(TArray<AHexGlobeTile*> longitudeHexes, APentGlobeTile* row1PentTile) {
	TArray<AHexGlobeTile*> tiles = {};

	FVector row1PentLocation = row1PentTile->GetActorLocation();
	FQuat row1PentRotation = row1PentTile->GetActorTransform().GetRotation();

	for (int i = 0; i < longitudeHexes.Num(); i++) {
		FVector hexPosition = longitudeHexes[i]->GetTransform().GetLocation();
		FQuat hexRotation = longitudeHexes[i]->GetTransform().GetRotation();

		FVector rotatedPosition = Util::RotateRelativeToVectorAndQuat(hexPosition, row1PentLocation, FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle)));
		FQuat rotatedRotation = FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle)) * hexRotation;

		AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, rotatedPosition, rotatedRotation.Rotator());
		tiles.Add(newHex);
	}

	return tiles;
}

APentGlobeTile* AGlobeMaker::CreateRow1Pent(double longitudeHexesDx, double longitudeHexesDz, int hexesBetweenPents) {
	double row1PentDx = cos(FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1))) * flatRadiusPent;
	double row1PentDz = sin(FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1))) * flatRadiusPent;

	FVector row1PentLocation = FVector(longitudeHexesDx + row1PentDx, 0, -1 * (longitudeHexesDz + row1PentDz));
	FQuat row1PentRotation = FQuat(FVector::YAxisVector, FMath::DegreesToRadians(outerDihedralAngle * (hexesBetweenPents + 1)));
	row1PentRotation = FQuat(row1PentRotation.GetAxisZ(), FMath::DegreesToRadians(centerPentagonAngle / 2)) * row1PentRotation;		//make the flats line up

	APentGlobeTile* row1Pent = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, row1PentLocation, row1PentRotation.Rotator());
	return row1Pent;
}

TArray<AHexGlobeTile*> AGlobeMaker::CreateInitialLongitudeHexes(APentGlobeTile* northPoleTile, int hexesBetweenPents, double& result_totalDx, double& result_totalDz){
	TArray<AHexGlobeTile*> tiles = {};

	double totalDx = flatRadiusPent;
	double totalDz = 0;
	for (int longitudeHexesPlaced = 0; longitudeHexesPlaced < hexesBetweenPents; longitudeHexesPlaced++) {
		double hexDx = cos(FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1))) * flatRadiusHex;
		double hexDz = sin(FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1))) * flatRadiusHex;

		totalDx += hexDx;
		totalDz += hexDz;

		FVector hexLocation = FVector(totalDx, 0, -1 * totalDz);
		FQuat hexRotation = FQuat(northPoleTile->GetTransform().GetRotation().GetAxisY(), FMath::DegreesToRadians(outerDihedralAngle * (longitudeHexesPlaced + 1)));
		hexRotation = FQuat(hexRotation.GetAxisZ(), FMath::DegreesToRadians(centerHexagonAngle / 2)) * hexRotation;		//make the flats line up

		AHexGlobeTile* newHex = GetWorld()->SpawnActor<AHexGlobeTile>(hexGlobeTile, hexLocation, hexRotation.Rotator());
		tiles.Add(newHex);

		//finish crossing the hexagon
		totalDx += hexDx;
		totalDz += hexDz;
	}

	result_totalDx = totalDx;
	result_totalDz = totalDz;
	return tiles;
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

