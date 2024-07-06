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
	dcel->adjacentVertices = GetVertexAdjacencies(dcel->vertices, dcel->originalVerticies);
	dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
	
	//2a) Do subdivisions here???
	for (int subdivisions = 0; subdivisions < 2; subdivisions++) {
		dcel->Subdivide();
		dcel->adjacentVertices = GetVertexAdjacencies(dcel->vertices, dcel->originalVerticies);
		dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(dcel->adjacentVertices);
	}

	//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
	//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
	//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	DoClockwiseAssignment(dcel->halfEdgesBetweenVertices, dcel->originalVerticies, false);

	//6) For every cycle, allocate and assign a face structure.*/
	TArray<face*> faces = GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("faces total = %d"), faces.Num());


//Get center of adjacent faces, make new list of vertices
	TArray<vertex*> hexGlobeVertices = {};
	TSet<face*> visitedFaces = {};
	for (int i = 0; i < dcel->vertices.Num(); i++) {
		vertex* v1 = dcel->vertices[i];
		TArray<face*> adjacentFaces = {};

		TArray<half_edge*> halfEdgesFacingAway = {};
		dcel->halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdgesFacingAway);
		for (int j = 0; j < halfEdgesFacingAway.Num(); j++) {
			if (!visitedFaces.Contains(halfEdgesFacingAway[j]->left)) {
				visitedFaces.Add(halfEdgesFacingAway[j]->left);

				adjacentFaces.Add(halfEdgesFacingAway[j]->left);
			}
		}

		for (int j = 0; j < adjacentFaces.Num(); j++) {
			face* adjacentFace = adjacentFaces[j];
			FVector center = FVector(0, 0, 0);
			for (int k = 0; k < adjacentFace->reps.Num(); k++) {
				center = center + adjacentFace->reps[k]->tail->location;
			}
			center /= adjacentFace->reps.Num();

			vertex* hexGlobeVertex = new vertex();
			hexGlobeVertex->location = center;
			hexGlobeVertex->name = adjacentFace->name;

			hexGlobeVertices.Add(hexGlobeVertex);
		}
	}
	//then do "3 adjacents" for the map
	TMap<vertex*, TArray<vertex*>> hexGlobeAdjacencies = GetHexGlobeAdjacencies(hexGlobeVertices);
	dcel->halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(hexGlobeAdjacencies);
	DoClockwiseAssignment(dcel->halfEdgesBetweenVertices, dcel->originalVerticies, true);
	faces = GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("globe faces total = %d"), faces.Num());


//Prepare vertices and triangles
	verticeLocations = {};
	for (int i = 0; i < hexGlobeVertices.Num(); i++) {
		hexGlobeVertices[i]->verticesIndex = i;
		verticeLocations.Add(hexGlobeVertices[i]->location);
	}

	triangles = {};
	for (int f = 0; f < faces.Num(); f++) {
		face* faceRef = faces[f];
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


TArray<face*> AProceduralGlobe::GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices) {
	TArray<half_edge*> allHalfEdges = {};
	
	//populateAll HalfEdges, effectively unpacking the nested map
	TArray<vertex*> outerKeys = {};
	halfEdgesBetweenVertices.GetKeys(outerKeys);
	for (int i = 0; i < outerKeys.Num(); i++) {
		vertex* outerKey = outerKeys[i];
		TArray<half_edge*> halfEdges = {};
		halfEdgesBetweenVertices[outerKey].GenerateValueArray(halfEdges);
		allHalfEdges.Append(halfEdges);
	}

	TArray<face*> faces = {};
	TSet<half_edge*> visitedEdges = {};

	for (int i = 0; i < allHalfEdges.Num(); i++) {
		if (!visitedEdges.Contains(allHalfEdges[i])) {
			half_edge* current_edge = allHalfEdges[i];

			face* newFace = new face();
			newFace->reps = {};
			newFace->name = "";

			while (!visitedEdges.Contains(current_edge)) {
				newFace->name += current_edge->name;

				newFace->reps.Add(current_edge);
				current_edge->left = newFace;
				visitedEdges.Add(current_edge);

				current_edge = current_edge->next;
			}

			faces.Add(newFace);
		}
	}


	return faces;
}

void AProceduralGlobe::LogVector(FVector in) {
	UE_LOG(LogTemp, Display, TEXT("%.2f,%.2f,%.2f"), in.X, in.Y, in.Z);
}

void AProceduralGlobe::DoClockwiseAssignment(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices, TSet<vertex*> originalVertices, bool isHexGlobe) {
//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	TArray<vertex*> vertices = {};
	halfEdgesBetweenVertices.GetKeys(vertices);

	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];
		UE_LOG(LogTemp, Display, TEXT("Clockwise for %s:"), *(v1->name));

		int adjacentVerticeCount;
		if (originalVertices.Contains(v1)) {
			adjacentVerticeCount = 5;
		}
		else {
			adjacentVerticeCount = 6;
		}
		if (isHexGlobe) {
			adjacentVerticeCount = 3;
		}

		TArray<half_edge*> halfEdges = {};
		halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdges);

		TArray<half_edge*> clockwiseEdges = {};

		TMap<FVector, half_edge*> half_edges_byTwinTailVector = {};
		//make half edges. Give them tails in clockwise order...
		for (int j = 0; j < adjacentVerticeCount; j++) {
			half_edge* he = halfEdges[j];
			FVector heNormalized = FVector(he->twin->tail->location);
			heNormalized.Normalize();
			half_edges_byTwinTailVector.Add(heNormalized, he);
		}

		half_edge* first = halfEdges[0];
		FVector firstNormalized = FVector(first->twin->tail->location);

		clockwiseEdges.Add(first);
		UE_LOG(LogTemp, Display, TEXT("first is %s"), *(first->name));

		//get the other side vectors of the half edges
		FVector vNormalized = FVector(v1->location);
		vNormalized.Normalize();

		//UE_LOG(LogTemp, Display, TEXT("v normalized:"));
		//LogVector(vNormalized);
		//UE_LOG(LogTemp, Display, TEXT("h0 normalized::"));
		//LogVector(he0Normalized);
		//UE_LOG(LogTemp, Display, TEXT("h1 normalized::"));
		//LogVector(he1Normalized);
		//UE_LOG(LogTemp, Display, TEXT("h2 normalized::"));
		//LogVector(he2Normalized);

		//get angles in clockwise order...?

		for (int j = 1; j < adjacentVerticeCount; j++) {
			TArray<FVector> halfEdgeVectors = {};
			half_edges_byTwinTailVector.GetKeys(halfEdgeVectors);

			//DEBUG HELPER: I've verified that 5 unique vectors come in
			int degreesToRotate = j * 360 / adjacentVerticeCount;
			UE_LOG(LogTemp, Display, TEXT("degrees to rotate: %d"), degreesToRotate);
			FVector expectedNextVector = Util::RotateRelativeToVectorAndQuat(firstNormalized, vNormalized, FQuat(vNormalized, FMath::DegreesToRadians(degreesToRotate)));
			expectedNextVector.Normalize();
			UE_LOG(LogTemp, Display, TEXT("expected rotation: "));
			LogVector(expectedNextVector);

			//after this for loop, closestVector is the next clockwise vector
			double closestSoFar = 9999;	//just bigger than 360 i guess
			FVector closestVector = FVector(0,0,0);	//it should never remain this, as we already added it above and are now rotating it
			for (int k = 0; k < halfEdgeVectors.Num(); k++) {
				FVector toMeasure = halfEdgeVectors[k];
				double offBy = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(expectedNextVector, toMeasure)));


				if (offBy < closestSoFar) {
					closestSoFar = offBy;
					closestVector = toMeasure;
				}
			}
			UE_LOG(LogTemp, Display, TEXT("next clockwise is %s, off by %.6f"), *(half_edges_byTwinTailVector[closestVector]->name), closestSoFar);
			UE_LOG(LogTemp, Display, TEXT("with vector:"));
			LogVector(closestVector);
			clockwiseEdges.Add(half_edges_byTwinTailVector[closestVector]);
		}


		for (int j = 0; j < clockwiseEdges.Num(); j++) {
			UE_LOG(LogTemp, Display, TEXT("\t%s"), *(clockwiseEdges[j]->name));

			half_edge* e1 = clockwiseEdges[j];
			half_edge* e2 = clockwiseEdges[(j+1) % clockwiseEdges.Num()];	//math to rotate the array to 0 at end

			e1->twin->next = e2;
			e2->prev = e1->twin;
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

				TArray<half_edge*> newHalfEdges = CreateHalfEdges(v1, v2);

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

TMap<vertex*, TArray<vertex*>> AProceduralGlobe::GetHexGlobeAdjacencies(TArray<vertex*> vertices) {
	TMap<vertex*, TArray<vertex*>> adjacencies = {};

	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];

		TMap<vertex*, double> edgeDistances = {};
		for (int j = 0; j < vertices.Num(); j++) {
			if (j != i) {
				vertex* v2 = vertices[j];
				double distance = (v1->location - v2->location).Length();
				edgeDistances.Add(v2, distance);
			}
		}

		edgeDistances.ValueSort([](double val1, double val2) {return val1 - val2 < 0; });
		TArray<vertex*> edgesByDist = {};
		edgeDistances.GenerateKeyArray(edgesByDist);

		int adjacentVerticeCount = 3;

		TArray<vertex*> adjacentVerts = {};
		for (int j = 0; j < adjacentVerticeCount; j++) {
			adjacentVerts.Add(edgesByDist[j]);
		}
		adjacencies.Add(v1, adjacentVerts);
	}

	return adjacencies;
}

TArray<half_edge*> AProceduralGlobe::CreateHalfEdges(vertex* vert1, vertex* vert2) {
	half_edge* e1 = new half_edge();
	half_edge* e2 = new half_edge();

	vert1->rep = e1;
	vert2->rep = e2;

	e1->tail = vert1;
	e2->tail = vert2;
	e1->twin = e2;
	e2->twin = e1;

	e1->name = e1->twin->tail->name;
	e2->name = e2->twin->tail->name;

	UE_LOG(LogTemp, Display, TEXT("creating half_edge with name %s"), *(e1->name));
	UE_LOG(LogTemp, Display, TEXT("creating half_edge with name %s"), *(e2->name));

	return {e1, e2};
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

