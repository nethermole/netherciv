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

void AProceduralGlobe::GenerateWorld() {

	UE_LOG(LogTemp, Display, TEXT("golden ratio = %f"), UE_DOUBLE_GOLDEN_RATIO);

	TArray<TArray<TArray<double>>> dodecahedronCartesianCoordinates =
	{
		{
			{1.0, -1.0},
			{1.0, -1.0},
			{1.0, -1.0},
		},
		{
			{0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{1.0 / UE_DOUBLE_GOLDEN_RATIO, -1.0 * 1.0 / UE_DOUBLE_GOLDEN_RATIO},
		},
		{
			{1.0 / UE_DOUBLE_GOLDEN_RATIO, -1.0 * 1.0 / UE_DOUBLE_GOLDEN_RATIO},
			{0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO}
		},
		{
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{1.0 / UE_DOUBLE_GOLDEN_RATIO, -1.0 * 1.0 / UE_DOUBLE_GOLDEN_RATIO},
			{0}
		}
	};

	int o = 0;
	int g = 1;
	int b = 2;
	int r = 3;

	TMap<int, FColor> colorMap = {};
	colorMap.Add(o, FColorList::Orange);
	colorMap.Add(g, FColorList::Green);
	colorMap.Add(b, FColorList::Blue);
	colorMap.Add(r, FColorList::Red);

	TArray<TArray<FVector>> v= {};	//vertex locations
	TArray<vertex*> vertices = {};


	for (int matrix = 0; matrix < 4; matrix++) {
		v.Add({});

		TArray<double> xCoords = dodecahedronCartesianCoordinates[matrix][0];
		TArray<double> yCoords = dodecahedronCartesianCoordinates[matrix][1];
		TArray<double> zCoords = dodecahedronCartesianCoordinates[matrix][2];

		int i = 0;
		for (int x = 0; x < xCoords.Num(); x++) {
			for (int y = 0; y < yCoords.Num(); y++) {
				for (int z = 0; z < zCoords.Num(); z++) {
					double xCoord = xCoords[x];
					double yCoord = yCoords[y];
					double zCoord = zCoords[z];

					FVector verticeLocation = FVector(xCoord, yCoord, zCoord);
					//verticeLocation = Util::RotateRelativeToVectorAndQuat(verticeLocation, FVector(0, 0, 0), FQuat(FVector::YAxisVector, FMath::DegreesToRadians(DIHEDRAL_ANGLE / 2)));
					verticeLocation = Util::GetVectorAtDistance(verticeLocation, 200);
					v[matrix].Add(verticeLocation);

					ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(verticeLocation));

					newSpherePoint->color = colorMap[matrix];
					newSpherePoint->index = i;



					vertex* newVertex = new vertex();
					newVertex->location = verticeLocation;

					if (colorMap[matrix] == FColorList::Orange) {
						newVertex->name = "Orange" + FString::FromInt(i);
					}
					if (colorMap[matrix] == FColorList::Green) {
						newVertex->name = "Green" + FString::FromInt(i);
					}
					if (colorMap[matrix] == FColorList::Blue) {
						newVertex->name = "Blue" + FString::FromInt(i);
					}
					if (colorMap[matrix] == FColorList::Red) {
						newVertex->name = "Red" + FString::FromInt(i);
					}
					newSpherePoint->SetActorLabel(newVertex->name);
					vertices.Add(newVertex);

					newSpherePoint->Initialize();

					i++;
				}
			}
		}

	}

	UE_LOG(LogTemp, Display, TEXT("created %d vertices"), vertices.Num());

//1) For each endpoint, create a vertex (we just did that)

//2) For each input segment, create two half-edges, and assign their tail vertices and twins.
	TMap<vertex*, TArray<vertex*>> adjacentVertices = GetVertexAdjacencies(vertices);
	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(adjacentVertices);


//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	DoClockwiseAssignment(halfEdgesBetweenVertices);

//6) For every cycle, allocate and assign a face structure.*/
	TArray<face*> faces = GetFacesFromHalfEdges(halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("faces total = %d"), faces.Num());
	for (int i = 0; i < faces.Num(); i++) {
		UE_LOG(LogTemp, Display, TEXT("facename = %s"), *(faces[i]->name));
		UE_LOG(LogTemp, Display, TEXT("\t"), *(faces[i]->rep->name));
		UE_LOG(LogTemp, Display, TEXT("\t"), *(faces[i]->rep2->name));
		UE_LOG(LogTemp, Display, TEXT("\t"), *(faces[i]->rep3->name));
		UE_LOG(LogTemp, Display, TEXT("\t"), *(faces[i]->rep4->name));
		UE_LOG(LogTemp, Display, TEXT("\t"), *(faces[i]->rep5->name));
	}

//Do subdivisions
	//int faceCount = faces.Num();
	//for (int f = 0; f < faceCount; f++) {
	//	face* faceRef = faces[f];

	//	FVector midpointLoc = FVector(0, 0, 0);
	//	for (int i = 0; i < faceRef->reps.Num(); i++) {
	//		midpointLoc += faceRef->reps[i]->tail->location;
	//	}
	//	midpointLoc /= faceRef->reps.Num();
	//	midpointLoc = Util::GetVectorAtDistance(midpointLoc, vertices[0]->location.Length());

	//	vertex* midpoint = new vertex();
	//	midpoint->location = midpointLoc;

	//	for (int e = 0; e < faceRef->reps.Num(); e++) {
	//		half_edge* edge = faceRef->reps[e];
	//	}
	//}


//Prepare vertices and triangles
	verticeLocations = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertices[i]->verticesIndex = i;
		verticeLocations.Add(vertices[i]->location);
	}

	triangles = {};
	for (int f = 0; f < faces.Num(); f++) {
		face* faceRef = faces[f];

		FVector midpoint = FVector(0,0,0);
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

TArray<face*> AProceduralGlobe::GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices) {
	TArray<half_edge*> allHalfEdges = {};
	
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
			face* newFace = new face();

			newFace->rep = allHalfEdges[i];
			newFace->rep2 = allHalfEdges[i]->next;
			newFace->rep3 = allHalfEdges[i]->next->next;
			newFace->rep4 = allHalfEdges[i]->next->next->next;
			newFace->rep5 = allHalfEdges[i]->next->next->next->next;

			newFace->reps = {};
			newFace->reps.Add(newFace->rep);
			newFace->reps.Add(newFace->rep2);
			newFace->reps.Add(newFace->rep3);
			newFace->reps.Add(newFace->rep4);
			newFace->reps.Add(newFace->rep5);

			newFace->name = newFace->rep->name;
			newFace->name += newFace->rep2->name;
			newFace->name += newFace->rep3->name;
			newFace->name += newFace->rep4->name;
			newFace->name += newFace->rep5->name;

			allHalfEdges[i]->left = newFace;
			allHalfEdges[i]->next->left = newFace;
			allHalfEdges[i]->next->next->left = newFace;
			allHalfEdges[i]->next->next->next->left = newFace;
			allHalfEdges[i]->next->next->next->next->left = newFace;

			visitedEdges.Add(allHalfEdges[i]);
			visitedEdges.Add(allHalfEdges[i]->next);
			visitedEdges.Add(allHalfEdges[i]->next->next);
			visitedEdges.Add(allHalfEdges[i]->next->next->next);
			visitedEdges.Add(allHalfEdges[i]->next->next->next->next);

			faces.Add(newFace);
		}
	}


	return faces;
}

void AProceduralGlobe::LogVector(FVector in) {
	UE_LOG(LogTemp, Display, TEXT("%.2f,%.2f,%.2f"), in.X, in.Y, in.Z);
}

void AProceduralGlobe::DoClockwiseAssignment(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices) {
//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	TArray<vertex*> vertices = {};
	halfEdgesBetweenVertices.GetKeys(vertices);

	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];

		TArray<half_edge*> halfEdges = {};
		halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdges);

		TArray<half_edge*> clockwiseEdges = {};

		half_edge* he0 = halfEdges[0];
		half_edge* he1 = halfEdges[1];
		half_edge* he2 = halfEdges[2];
		clockwiseEdges.Add(he0);

		FVector vNormalized = FVector(v1->location);
		FVector h0Normalized = FVector(he0->twin->tail->location);
		FVector h1Normalized = FVector(he1->twin->tail->location);
		FVector h2Normalized = FVector(he2->twin->tail->location);
		vNormalized.Normalize();
		h0Normalized.Normalize();
		h1Normalized.Normalize();
		h2Normalized.Normalize();

		UE_LOG(LogTemp, Display, TEXT("v normalized:"));
		LogVector(vNormalized);
		UE_LOG(LogTemp, Display, TEXT("h0 normalized::"));
		LogVector(h0Normalized);
		UE_LOG(LogTemp, Display, TEXT("h1 normalized::"));
		LogVector(h1Normalized);
		UE_LOG(LogTemp, Display, TEXT("h2 normalized::"));
		LogVector(h2Normalized);

		FVector expectedSecondVector = Util::RotateRelativeToVectorAndQuat(h0Normalized, FVector(0,0,0), FQuat(vNormalized, FMath::DegreesToRadians(360 / 3)));
		expectedSecondVector.Normalize();

		UE_LOG(LogTemp, Display, TEXT("expectedSecondVector:"));
		LogVector(expectedSecondVector);

		double he1Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(expectedSecondVector, h1Normalized)));
		double he2Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(expectedSecondVector, h2Normalized)));

		UE_LOG(LogTemp, Display, TEXT("%s angle: %f"), *(he0->name), 0);
		UE_LOG(LogTemp, Display, TEXT("%s h1angle from predicted next, clockwise: %f"), *(he1->name), he1Angle);
		UE_LOG(LogTemp, Display, TEXT("%s h1angle from predicted next, clockwise: %f"), *(he2->name), he2Angle);
		
		if (he1Angle < he2Angle) {
			clockwiseEdges.Add(he1);
			clockwiseEdges.Add(he2);
		}
		else {
			clockwiseEdges.Add(he2);
			clockwiseEdges.Add(he1);
		}
		

		for (int j = 0; j < clockwiseEdges.Num(); j++) {
			half_edge* e1 = clockwiseEdges[j];
			half_edge* e2 = clockwiseEdges[(j+1)%3];

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

		for (int j = 0; j < 3; j++) {
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

TMap<vertex*, TArray<vertex*>> AProceduralGlobe::GetVertexAdjacencies(TArray<vertex*> vertices) {
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

		adjacencies.Add(v1, { edgesByDist[0], edgesByDist[1], edgesByDist[2] });
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

