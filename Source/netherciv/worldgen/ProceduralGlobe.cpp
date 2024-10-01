// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGlobe.h"

#include "netherciv/util/Util.h"

#include "math.h"

// Sets default values
AProceduralGlobe::AProceduralGlobe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProceduralGlobe::GenerateWorld(int subdivisions) 
{
	DoublyConnectedEdgeList* dcel = new DoublyConnectedEdgeList();
	if (subdivisions > 8) {
		UE_LOG(LogTemp, Display, TEXT("Cannot generate world with more than 8 subdivisions yet"));
		return;
	}
	dcel->ReadFromFile(subdivisions);
	dcel->PrepareVerticeLocationsAndTriangles();

	faceCount = dcel->faces.Num();

	verticeLocations = dcel->verticeLocations;
	triangles = dcel->triangles;

	allTrianglesBy3s = dcel->trianglesBy3s;
	waterTriangles = dcel->waterTrianglesBy3s;
	landTriangles = dcel->landTrianglesBy3s;

	allVerticeLocations = dcel->allVerticeLocations;
	allTriangles = dcel->allTriangles;

	
//Use this to generate new files to read
	//for (int subs = 0; subs < 9; subs++) {
	//	CreateGlobeDcel(subs);
	//	dcel->WriteToFile(subs);
	//}

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
void AProceduralGlobe::CALCULATEGLOBE_CreateGlobeDcel(int subdivisions)
{
	TRACE_BOOKMARK(TEXT("CreateGlobeDcel bookmark"))
	TRACE_CPUPROFILER_EVENT_SCOPE(AProceduralGlobe::CALCULATEGLOBE_CreateGlobeDcel)
//Load the subdivided icosahedron
	dcel_property = new DoublyConnectedEdgeList();

	dcel_property->LoadIcosahedronCartesianCoordinates();	//1
	dcel_property->CalculateHalfEdges(false);		//2

	for (int subdivCount = 0; subdivCount < subdivisions; subdivCount++) {	//3
		dcel_property->Subdivide();
		dcel_property->CalculateHalfEdges(false);
	}
	dcel_property->DoClockwiseAssignment(false);		//4
	dcel_property->GetFacesFromHalfEdges(dcel_property->halfEdgesBetweenVertices);	//5


//To hexes
	DoublyConnectedEdgeList* hexDcel = dcel_property->CreateGoldbergPolyhedronFromSubdividedIcosahedron();
	hexDcel->PrepareVerticeLocationsAndTriangles();
	dcel_property = hexDcel;
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

