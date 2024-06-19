// Fill out your copyright notice in the Description page of Project Settings.

#include "GlobeMaker.h"

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
	UE_LOG(LogTemp, Display, TEXT("drawing board"));

	TArray<APentGlobeTile*> pentTiles = {};



	int baseHeight = 500;

	int centerHexAngle = 360 / 6;	//60
	double halfCornerAngle = 180 - 90 - (centerHexAngle / 2);	//also 60
	double flatRadiusHex = 1.0;
	double radiusHex = 1 / sin(FMath::DegreesToRadians(halfCornerAngle));
	double edgeLength = 2 * (1 / tan(FMath::DegreesToRadians(60)));

	int centerPentAngle = 360 / 5;					//72
	int halfCenterPentAngle = centerPentAngle / 2;	//36
	double flatRadiusPent = (edgeLength / 2) / tan(FMath::DegreesToRadians(halfCenterPentAngle));
	double radiusPent = (edgeLength / 2) / sin(FMath::DegreesToRadians(halfCenterPentAngle));

	double flatRadiusSphere = tan(FMath::DegreesToRadians(DIHEDRAL_ANGLE/2))*flatRadiusPent;

	FVector sphereCenter = FVector(0, 0, baseHeight - (flatRadiusSphere * 100));


	UE_LOG(LogTemp, Display, TEXT("flatRadiusHex=%f"), flatRadiusHex);
	UE_LOG(LogTemp, Display, TEXT("radiusHex=%f"), radiusHex);
	UE_LOG(LogTemp, Display, TEXT("edgeLength=%f"), edgeLength);
	UE_LOG(LogTemp, Display, TEXT("flatRadiusPent=%f"), flatRadiusPent);
	UE_LOG(LogTemp, Display, TEXT("radiusPent=%f"), radiusPent);
	UE_LOG(LogTemp, Display, TEXT("flatRadiusSphere=%f"), flatRadiusSphere);



	FActorSpawnParameters spawnInfo;

	double dihedralAngle = 116.565;
	int degreesInCircle = 360;

	double additionalRadiusToPent = cos(180 - 90 - (DIHEDRAL_ANGLE / 2)) * 2 * flatRadiusHex;



	//North Pole
	APentGlobeTile* northPole = GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, baseHeight)));
	pentTiles.Add(northPole);


	double dihedralBeyond90 = dihedralAngle - 90;
	FQuat dihedral = FQuat(FVector(0, 1, 0), FMath::DegreesToRadians(-1 * dihedralAngle));
	FQuat flipped = FQuat(dihedral.GetForwardVector(), FMath::DegreesToRadians(180)) * dihedral;

	double centerXDiff = (sin(FMath::DegreesToRadians(dihedralBeyond90)) * flatRadiusPent + flatRadiusPent) * 100;
	double centerYDiff = cos(FMath::DegreesToRadians(dihedralBeyond90)) * flatRadiusPent * 100 * -1;

	for (int i = 0; i < 5; i += 1) {
		FQuat positionRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians((360 / 5) * i));
		//Draw hexes to adjacent pent



		// Add adjacent pent
		FVector position = positionRotation.RotateVector(FVector(centerXDiff, 0, centerYDiff + baseHeight));
		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, position, (positionRotation * flipped).Rotator()));

		
		FQuat middleFlip = FQuat(FVector::RightVector, FMath::DegreesToRadians(180));

		FVector newPos = middleFlip.RotateVector(position - sphereCenter);

		FQuat aroundY180 = FQuat(FVector::RightVector, FMath::DegreesToRadians(180));

		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, newPos + sphereCenter, (aroundY180 * positionRotation * flipped).Rotator()));

	}

	//South Pole
	pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(0, 0, baseHeight - 2 * flatRadiusSphere * 100), FQuat(FVector::RightVector, FMath::DegreesToRadians(180)).Rotator()));

	UE_LOG(LogTemp, Display, TEXT("pentTiles[0].Z=%f"), pentTiles[0]->GetTransform().GetLocation().Z);
	UE_LOG(LogTemp, Display, TEXT("pentTiles num=%d"), pentTiles.Num());


	for (int i = 0; i < 12; i++) {
		APentGlobeTile* pentTile = pentTiles[i];

		FVector actorFromCenter = pentTile->GetActorLocation() - sphereCenter;
		actorFromCenter.Normalize();
		actorFromCenter.X *= (additionalRadiusToPent*100);
		actorFromCenter.Y *= (additionalRadiusToPent*100);
		actorFromCenter.Z *= (additionalRadiusToPent*100);
		


		pentTiles.Add(GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, pentTile->GetActorLocation() + actorFromCenter, pentTile->GetActorRotation()));
		//pentTile->SetActorLocation(pentTile->GetActorLocation() + pentTile->GetActorLocation());
	}




	////North Pole
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, baseHeight)));


	//double dihedralBeyond90 = dihedralAngle - 90;
	//FQuat dihedral = FQuat(FVector(0,1,0), FMath::DegreesToRadians(-1 * dihedralAngle));
	//FQuat flipped = FQuat(dihedral.GetForwardVector(), FMath::DegreesToRadians(180)) * dihedral;

	//double centerXDiff = (sin(FMath::DegreesToRadians(dihedralBeyond90)) * flatRadiusPent + flatRadiusPent) * 100;
	//double centerYDiff = cos(FMath::DegreesToRadians(dihedralBeyond90)) * flatRadiusPent * 100 * -1;

	//for (int i = 0; i < 5; i += 1) {
	//	FQuat positionRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians((360 / 5) * i));
	//	FVector position = positionRotation.RotateVector(FVector(centerXDiff, 0, centerYDiff + baseHeight));

	//	GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, position, (positionRotation * flipped).Rotator());



	//	FVector sphereCenter = FVector(0, 0, baseHeight - (flatRadiusSphere * 100));
	//	FQuat middleFlip = FQuat(FVector::RightVector, FMath::DegreesToRadians(180));

	//	FVector newPos = middleFlip.RotateVector(position - sphereCenter);

	//	FQuat aroundY180 = FQuat(FVector::RightVector, FMath::DegreesToRadians(180));

	//	GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, newPos + sphereCenter, (aroundY180 * positionRotation * flipped).Rotator());

	//}

	////South Pole
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(0, 0, baseHeight - 2*flatRadiusSphere*100), FQuat(FVector::RightVector, FMath::DegreesToRadians(180)).Rotator());






////	Works...
// 
// 
//	//bottom pentagon
//	GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(0, 0, 0)));
//
//
//	int numSides = 5;
//	int middleAngleDegrees = 360 / numSides;									//72
//	int halfMiddleAngleDegrees = middleAngleDegrees / 2;						//36
//
//	//(clockwise, base index 0, top right)
//	int a0_YaxisRight_00corner = middleAngleDegrees;							//72
//	int a1_XaxisAbove_00corner = 90.0 - a0_YaxisRight_00corner;					//18
//
//	double a1_Xcord = cos(a1_XaxisAbove_00corner*PI/180);						//~0.951
//	double a1_Ycord = sin(a1_XaxisAbove_00corner*PI/180);						//~0.309
//
////	UE_LOG(LogTemp, Display, TEXT("a1_Xcord: %f"), a1_Xcord);
////	UE_LOG(LogTemp, Display, TEXT("a1_Ycord: %f"), a1_Ycord);
//
//	double a1_Xcord_edgeMidpoint = a1_Xcord / 2;
//	double a1_Ycord_edgeMidpoint = (a1_Ycord + 1) / 2;
//
//	double edgeDistance = sqrt((a1_Xcord_edgeMidpoint * a1_Xcord_edgeMidpoint) + (a1_Ycord_edgeMidpoint * a1_Ycord_edgeMidpoint));
//
//
//	double adjacentAngleOffset = 180.0 - DIHEDRAL_ANGLE;	//~64
//	double a0_adjacentAngle_zCord = sin(adjacentAngleOffset * PI / 180) * edgeDistance;
//
//
//	double a1_adjacentAngle_xyDistance = edgeDistance + (cos(adjacentAngleOffset * PI / 180)*edgeDistance);
//
//
//	UE_LOG(LogTemp, Display, TEXT("a1_adjacentAngle_xyDistance: %f"), a1_adjacentAngle_xyDistance);
//
//	double a0_YaxisRight_angleToAdjacentCenter = halfMiddleAngleDegrees;		//36
//
//	double a0_adjacentAngle_xCord = sin(a0_YaxisRight_angleToAdjacentCenter * PI / 180) * a1_adjacentAngle_xyDistance;
//	double a0_adjacentAngle_yCord = cos(a0_YaxisRight_angleToAdjacentCenter * PI / 180) * a1_adjacentAngle_xyDistance;
//
//	double a1_a0_YaxisRight_angleToAdjacentCenter = halfMiddleAngleDegrees + middleAngleDegrees;
//
//	UE_LOG(LogTemp, Display, TEXT("sin(a0_YaxisRight_angleToAdjacentCenter): %f"), sin(a0_YaxisRight_angleToAdjacentCenter * PI / 180));
//	UE_LOG(LogTemp, Display, TEXT("cos(a0_YaxisRight_angleToAdjacentCenter): %f"), cos(a0_YaxisRight_angleToAdjacentCenter * PI / 180));
//
//
//	UE_LOG(LogTemp, Display, TEXT("a0_adjacentAngle_xCord: %f"), a0_adjacentAngle_xCord);
//	UE_LOG(LogTemp, Display, TEXT("a0_adjacentAngle_yCord: %f"), a0_adjacentAngle_yCord);
//	UE_LOG(LogTemp, Display, TEXT("a0_adjacentAngle_zCord: %f"), a0_adjacentAngle_zCord);
////
//	FVector a0_adjacent_location = FVector(a0_adjacentAngle_xCord * 100, a0_adjacentAngle_yCord * -100, a0_adjacentAngle_zCord * 100);
//	FRotator a0_adjacent_rotation = FRotator(0, halfMiddleAngleDegrees, 180 - DIHEDRAL_ANGLE);
//	GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, a0_adjacent_location, a0_adjacent_rotation);






	//hand-placed
	//int locX = 700;
	//int row1Height = 75;
	//int row2Height = 195;


	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FTransform(FVector(locX,0,0)));

	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX + 70, -100, row1Height), FRotator(0, 216, dihedralAngle), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX + 115, 36, row1Height), FRotator(0, 288, dihedralAngle), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX, 120, row1Height), FRotator(0, 0, dihedralAngle), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX -115, 36, row1Height), FRotator(0, 72, dihedralAngle), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX - 70, -100, row1Height), FRotator(0, 144, dihedralAngle), spawnInfo);

	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX + 115, -40, row2Height), FRotator(0, 216+36, dihedralAngle*-1), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX + 70, 95, row2Height), FRotator(0, 216 + 36+72, dihedralAngle * -1), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX - 70, 95, row2Height), FRotator(0, 216 + 36 + 72+72, dihedralAngle * -1), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX - 115, -40, row2Height), FRotator(0, 216 + 36 + 72 + 72+72, dihedralAngle * -1), spawnInfo);
	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX, -120, row2Height), FRotator(0, 216 + 36 + 72 + 72 + 72+72, dihedralAngle * -1), spawnInfo);

	//GetWorld()->SpawnActor<APentGlobeTile>(pentGlobeTile, FVector(locX, 0, 265), FRotator(0, 36, 0));




	//GetWorld()->SpawnActor<AGlobeTile>(globeTile, FTransform(FVector(300,0,50)));

	UE_LOG(LogTemp, Display, TEXT("done drawing board"));
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

