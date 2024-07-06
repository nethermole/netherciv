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
	dcel->adjacentVertices = GetVertexAdjacencies(dcel->vertices, dcel->originalVertices);
	dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
	
	//2a) Do subdivisions here???
	for (int subdivisions = 0; subdivisions < 2; subdivisions++) {
		dcel->Subdivide();
		dcel->adjacentVertices = GetVertexAdjacencies(dcel->vertices, dcel->originalVertices);
		dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
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
	dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(dcel->hexGlobeAdjacencies);
	dcel->DoClockwiseAssignment(true);
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("globe faces total = %d"), dcel->faces.Num());


//Prepare vertices and triangles
	verticeLocations = {};
	for (int i = 0; i < dcel->hexGlobeVertices.Num(); i++) {
		dcel->hexGlobeVertices[i]->verticesIndex = i;
		verticeLocations.Add(dcel->hexGlobeVertices[i]->location);
	}

	triangles = {};
	for (int f = 0; f < dcel->faces.Num(); f++) {
		face* faceRef = dcel->faces[f];
		if (faceRef->reps.Num() == 3) {
			triangles.Append({
				faceRef->reps[0]->tail->verticesIndex,
				faceRef->reps[1]->tail->verticesIndex,
				faceRef->reps[2]->tail->verticesIndex
				});
		}
		else {
			FVector midpoint = FVector(0, 0, 0);
			for (int i = 0; i < faceRef->reps.Num(); i++) {
				midpoint += faceRef->reps[i]->tail->location;
			}
			midpoint /= faceRef->reps.Num();
			verticeLocations.Add(midpoint);

			for (int i = 0; i < faceRef->reps.Num(); i++) {
				half_edge* edge = faceRef->reps[i];

				triangles.Append({
					edge->tail->verticesIndex,
					edge->next->tail->verticesIndex,
				verticeLocations.Num() - 1
					});
			}
		}
	}
}


TMap<vertex*, TMap<vertex*, half_edge*>> AProceduralGlobe::GetHalfEdgesBetweenVertices(TMap<vertex*, TArray<vertex*>> adjacentVertices) {
	TArray<vertex*> vertices = {};
	adjacentVertices.GetKeys(vertices);

	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];
		if (!halfEdgesBetweenVertices.Contains(v1)) {
			halfEdgesBetweenVertices.Add(v1, {});
		}

		for (int j = 0; j < adjacentVertices[v1].Num(); j++) {
			vertex* v2 = adjacentVertices[v1][j];

			if (!halfEdgesBetweenVertices[v1].Contains(v2)) {
				if (!halfEdgesBetweenVertices.Contains(v2)) {
					halfEdgesBetweenVertices.Add(v2, {});
				}

				TArray<half_edge*> newHalfEdges = DoublyConnectedEdgeList::CreateHalfEdges(v1, v2);

				halfEdgesBetweenVertices[v1].Add(v2, newHalfEdges[0]);
				halfEdgesBetweenVertices[v2].Add(v1, newHalfEdges[1]);
			}
		}
	}
	return halfEdgesBetweenVertices;
}

TMap<vertex*, TArray<vertex*>> AProceduralGlobe::GetVertexAdjacencies(TArray<vertex*> vertices, TSet<vertex*> originalVertices) {
	TMap<vertex*, TArray<vertex*>> adjacencies = {};

	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];

		TMap<vertex*, double> edgeDistances = {};
		for (int j = 0;j < vertices.Num(); j++) {
			if (j != i) {
				vertex* v2 = vertices[j];
				double distance = (v1->location - v2->location).Length();
				edgeDistances.Add(v2, distance);
			}
		}

		edgeDistances.ValueSort([](double val1, double val2) {return val1 - val2 < 0; });
		TArray<vertex*> edgesByDist = {};
		edgeDistances.GenerateKeyArray(edgesByDist);

		int adjacentVerticeCount;
		if (originalVertices.Contains(v1)) {
			adjacentVerticeCount = 5;
		}
		else {
			adjacentVerticeCount = 6;
		}

		TArray<vertex*> adjacentVerts = {};
		for (int j = 0; j < adjacentVerticeCount; j++) {
			adjacentVerts.Add(edgesByDist[j]);
		}
		adjacencies.Add(v1, adjacentVerts);
	}
	
	return adjacencies;
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

