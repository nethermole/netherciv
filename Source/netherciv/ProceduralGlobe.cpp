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
	dcel = new DoublyConnectedEdgeList();
	dcel->LoadIcosahedronCartesianCoordinates();

	//2) For each input segment, create two half-edges, and assign their tail vertices and twins.
	dcel->adjacentVertices = dcel->GetVertexAdjacencies(dcel->vertices);
	dcel->halfEdgesBetweenVertices = dcel->GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
	
	//2a) Do subdivisions here???
	for (int subdivisions = 0; subdivisions < 2; subdivisions++) {
		dcel->Subdivide();
		dcel->adjacentVertices = dcel->GetVertexAdjacencies(dcel->vertices);
		dcel->halfEdgesBetweenVertices = dcel->GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
	}

	//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
	//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
	//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	dcel->DoClockwiseAssignment(false);

	//6) For every cycle, allocate and assign a face structure.*/
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("faces total = %d"), dcel->faces.Num());


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

	verticeLocations = dcel->verticeLocations;
	triangles = dcel->triangles;
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

