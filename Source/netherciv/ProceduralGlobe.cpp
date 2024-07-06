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

	//1) For each endpoint, create a vertex.
	dcel->LoadIcosahedronCartesianCoordinates();

	//2)For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
	dcel->CalculateHalfEdges();

	//3) Subdivide n times
	for (int subdivCount = 0; subdivCount < subdivisions; subdivCount++) {
		//3a) Generate new vertices
		dcel->Subdivide();
		//3b) For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
		dcel->CalculateHalfEdges();
	}

	//4)For each endpoint, sort the half - edges whose tail vertex is that endpoint in clockwise order.
	//	For every pair of half - edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin
	dcel->DoClockwiseAssignment(false);

	//6)For every cycle, allocate and assign a face structure.
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);

//To hexes
	//Get center of adjacent faces, make new list of vertices for the hex globe
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

