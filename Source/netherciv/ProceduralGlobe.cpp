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
	DoublyConnectedEdgeList* dcel2 = new DoublyConnectedEdgeList();
	dcel2->ReadFromFile(1);
	dcel2->PrepareVerticeLocationsAndTrianglesAndUV0s();

	faceCount = dcel2->faces.Num();

	verticeLocations = dcel2->verticeLocations;
	triangles = dcel2->triangles;
	uv0s = dcel2->uv0s;
	allTrianglesBy3s = dcel2->trianglesBy3s;
	waterTriangles = dcel2->waterTrianglesBy3s;
	landTriangles = dcel2->landTrianglesBy3s;

	allVerticeLocations = dcel2->allVerticeLocations;
	allTriangles = dcel2->allTriangles;
	allUV0 = dcel2->allUv0s;

	


	//CreateGlobeDcel(9);
	//dcel->WriteToFile(9);

}


//1) For each endpoint, create a vertex.
//2)For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
//3) Subdivide n times
	//3a) Generate new vertices
	//3b) For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
//4)
	//4a) For each endpoint, sort the half - edges whose tail vertex is that endpoint in clockwise order.
	//4b) For every pair of half - edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin
//5)For every cycle, allocate and assign a face structure.
void AProceduralGlobe::CreateGlobeDcel(int subdivisions)
{
	TRACE_BOOKMARK(TEXT("CreateGlobeDcel bookmark"))
	TRACE_CPUPROFILER_EVENT_SCOPE(AProceduralGlobe::CreateGlobeDcel)
//Load the subdivided icosahedron
	dcel = new DoublyConnectedEdgeList();

	dcel->LoadIcosahedronCartesianCoordinates();	//1
	dcel->CalculateHalfEdges(false);		//2

	for (int subdivCount = 0; subdivCount < subdivisions; subdivCount++) {	//3
		dcel->Subdivide();
		dcel->CalculateHalfEdges(false);
	}
	dcel->DoClockwiseAssignment(false);		//4
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);	//5


//To hexes
	DoublyConnectedEdgeList* hexDcel = dcel->CreateGoldbergPolyhedronFromSubdividedIcosahedron();
	hexDcel->PrepareVerticeLocationsAndTrianglesAndUV0s();
	dcel = hexDcel;
}

TArray<FVector> AProceduralGlobe::GetAllVerticeLocations()
{
	return allVerticeLocations;
}

TArray<int> AProceduralGlobe::GetAllTriangles()
{
	return allTriangles;
}

TArray<FIntVector> AProceduralGlobe::GetAllTrianglesBy3s()
{
	return allTrianglesBy3s;
}

TArray<FIntVector> AProceduralGlobe::GetAllWaterTrianglesBy3s()
{
	return waterTriangles;
}

TArray<FIntVector> AProceduralGlobe::GetAllLandTrianglesBy3s()
{
	return landTriangles;
}

TArray<FVector2D> AProceduralGlobe::GetAllUV0()
{
	return allUV0;
}

TArray<FVector> AProceduralGlobe::GetVerticeLocationsByFaceIndex(int index)
{
	return verticeLocations[index];
}

TArray<int> AProceduralGlobe::GetTrianglesByFaceIndex(int index)
{
	return triangles[index];
}

TArray<FVector2D> AProceduralGlobe::getUV0ByFaceIndex(int index)
{
	return uv0s[index];
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

