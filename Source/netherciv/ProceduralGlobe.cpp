// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGlobe.h"

#include "Util.h"

#include "math.h"

#define DIHEDRAL_ANGLE 116.56505

// Sets default values
AProceduralGlobe::AProceduralGlobe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProceduralGlobe::GenerateWorld() 
{
	CreateGlobeDcel(2);

	verticeLocations = dcel->verticeLocations;
	triangles = dcel->triangles;
}

void AProceduralGlobe::CreateGlobeDcel(int subdivisions)
{
	dcel = new DoublyConnectedEdgeList();
	dcel->LoadIcosahedronCartesianCoordinates();
	dcel->CalculateHalfEdges();

	//2a) Do subdivisions here???
	for (int subdivCount = 0; subdivCount < subdivisions; subdivCount++) {
		dcel->Subdivide();
		dcel->CalculateHalfEdges();
	}

	dcel->DoClockwiseAssignment(false);
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);


	//Get center of adjacent faces, make new list of vertices
	dcel->hexGlobeVertices = dcel->GenerateHexGlobeVertices();

	//then do "3 adjacents" for the map
	dcel->hexGlobeAdjacencies = dcel->GetHexGlobeAdjacencies(dcel->hexGlobeVertices);
	dcel->halfEdgesBetweenVertices = dcel->GetHalfEdgesBetweenVertices(dcel->hexGlobeAdjacencies);
	dcel->DoClockwiseAssignment(true);
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("globe faces total = %d"), dcel->faces.Num());


	//Prepare verticesLocations and triangles
	dcel->PrepareVerticeLocationsAndTriangles();
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

