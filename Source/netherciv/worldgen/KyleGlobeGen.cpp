// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/KyleGlobeGen.h"

KyleGlobeGen::KyleGlobeGen()
{
}

KyleGlobeGen::~KyleGlobeGen()
{
}

void KyleGlobeGen::OuputGlobeFilesUpToNSubdivisions(int maxSubdivisions) {
	for (int i = 0; i <= maxSubdivisions; i++) {
		OutputGlobeFile(i);
	}
}

void KyleGlobeGen::OutputGlobeFile(int subdivisions) {
		CreateGlobeDcel(subdivisions);
		dcel->WriteToFile(subdivisions);
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
void KyleGlobeGen::CreateGlobeDcel(int subdivisions)
{
	TRACE_BOOKMARK(TEXT("CreateGlobeDcel bookmark"))
	TRACE_CPUPROFILER_EVENT_SCOPE(AProceduralGlobe::CALCULATEGLOBE_CreateGlobeDcel)
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
	hexDcel->PrepareVerticeLocationsAndTriangles();
	dcel = hexDcel;
}

void KyleGlobeGen::LoadGlobeFromFile(int subdivisions)
{
	dcel = new DoublyConnectedEdgeList();
	if (subdivisions > 8) {
		UE_LOG(LogTemp, Display, TEXT("Cannot generate world with more than 8 subdivisions yet"));
		return;
	}
	dcel->ReadFromFile(subdivisions);
	dcel->PrepareVerticeLocationsAndTriangles();
}
