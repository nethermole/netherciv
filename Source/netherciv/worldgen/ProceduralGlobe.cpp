// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGlobe.h"

#include "math.h"

#include "netherciv/util/Util.h"

// Sets default values
AProceduralGlobe::AProceduralGlobe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProceduralGlobe::GenerateWorld(int subdivisions) 
{
	kyleGlobeGen = new KyleGlobeGen();
	kyleGlobeGen->LoadGlobeFromFile(subdivisions);

	//dadGlobeGen = new DadGlobeGen();
	//dadGlobeGen->Prepare(subdivisions);

	faceCount = kyleGlobeGen->dcel->faces.Num();

	UE_LOG(LogTemp, Display, TEXT("Done loading world in code, now blueprint time"));
}


TArray<FVector> AProceduralGlobe::GetAllVerticeLocations()
{
	//return dadGlobeGen->GetAllVerticeLocations();
	return kyleGlobeGen->dcel->allVerticeLocations;
}

TArray<FIntVector> AProceduralGlobe::GetTriangleIntVectorsForFaceByIndex(int faceIndex) {
	return kyleGlobeGen->dcel->GetTriangleIntVectorsForFaceByIndex(faceIndex);
}

bool AProceduralGlobe::isFaceWater(int faceIndex) {
	return kyleGlobeGen->dcel->faces[faceIndex]->isWater;
}

int AProceduralGlobe::GetFaceIDOfInitialHumanSpawnableLocation() {
	int maxFaceIndex = kyleGlobeGen->dcel->faces.Num()-1;

	for (int i = 0; i < 1000000; i++) {
		int faceToCheck = FMath::RandRange(0, maxFaceIndex);
		if (!kyleGlobeGen->dcel->faces[faceToCheck]->isWater) {
			return faceToCheck;
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Failed to find place to spawn initial human"));
	return -1;
}

FVector AProceduralGlobe::GetMidpointOfFace(int faceId) {
	return kyleGlobeGen->dcel->GetMidpointOfFace(faceId);
}

TArray<int> AProceduralGlobe::GetAdjacentFaceIDs(int faceID)
{
	return kyleGlobeGen->GetAdjacentFaceIDs(faceID);
}

TArray<int> AProceduralGlobe::GetAdjacentLandFaceIDs(int faceID)
{
	return kyleGlobeGen->GetAdjacentLandFaceIDs(faceID);
}


TArray<FIntVector> AProceduralGlobe::GetAllWaterTrianglesBy3s()
{
	//return dadGlobeGen->GetWaterTrianglesBy3s();
	return kyleGlobeGen->dcel->waterTrianglesBy3s;
}

TArray<FIntVector> AProceduralGlobe::GetAllLandTrianglesBy3s()
{
	//return dadGlobeGen->GetLandTrianglesBy3s();
	return kyleGlobeGen->dcel->landTrianglesBy3s;
}

void AProceduralGlobe::GenerateKyleGlobeGenFilesUpToNSubdivisions(int subdivisions)
{
	kyleGlobeGen = new KyleGlobeGen();
	kyleGlobeGen->OuputGlobeFilesUpToNSubdivisions(subdivisions);
}

void AProceduralGlobe::GenerateDadGlobeGenAtNSubdivisions(int subdivisions)
{
	dadGlobeGen = new DadGlobeGen();
	dadGlobeGen->Prepare(subdivisions);
}

int AProceduralGlobe::GetFaceCount()
{
	return faceCount;
}

// Called when the game starts or when spawned
void AProceduralGlobe::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProceduralGlobe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

