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

	/*TArray<TArray<TArray<double>>> dodecahedronCartesianCoordinates =
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
	};*/

	TArray<TArray<TArray<double>>> icosahedronCartesianCoordinates =
	{
		{
			{0},
			{1.0, -1.0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
		},
		{
			{1.0, -1.0},
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{0},
		},
		{
			{UE_DOUBLE_GOLDEN_RATIO, -1.0 * UE_DOUBLE_GOLDEN_RATIO},
			{0},
			{1.0, -1.0}
		}
	};

	TArray<TArray<FVector>> v= {};	//vertex locations
	TArray<vertex*> vertices = {};
	TSet<vertex*> originalVerticies = {};

	int counter = 0;
	for (int matrix = 0; matrix < 3; matrix++) {
		v.Add({});

		TArray<double> xCoords = icosahedronCartesianCoordinates[matrix][0];
		TArray<double> yCoords = icosahedronCartesianCoordinates[matrix][1];
		TArray<double> zCoords = icosahedronCartesianCoordinates[matrix][2];

		
		for (int x = 0; x < xCoords.Num(); x++) {
			for (int y = 0; y < yCoords.Num(); y++) {
				for (int z = 0; z < zCoords.Num(); z++) {
					double xCoord = xCoords[x];
					double yCoord = yCoords[y];
					double zCoord = zCoords[z];

					FVector verticeLocation = FVector(xCoord, yCoord, zCoord);
					//verticeLocation = Util::RotateRelativeToVectorAndQuat(verticeLocation, FVector(0, 0, 0), FQuat(FVector::YAxisVector, FMath::DegreesToRadians(DIHEDRAL_ANGLE / 2)));
					verticeLocation = Util::GetVectorAtDistance(verticeLocation, 1000);
					v[matrix].Add(verticeLocation);

					ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(verticeLocation));
					newSpherePoint->floatingLabel =  FString::FromInt(counter);
					counter++;

					vertex* newVertex = new vertex();
					newVertex->location = verticeLocation;
					newVertex->name = newSpherePoint->floatingLabel;

					vertices.Add(newVertex);
					originalVerticies.Add(newVertex);

					if (matrix == 0) {
						newSpherePoint->color = FColor::Red;
					}
					if (matrix == 1) {
						newSpherePoint->color = FColor::Yellow;
					}
					if (matrix == 2) {
						newSpherePoint->color = FColor::Blue;
					}

					newSpherePoint->Initialize();
				}
			}
		}
		
	}

	UE_LOG(LogTemp, Display, TEXT("created %d vertices"), vertices.Num());

//1) For each endpoint, create a vertex (we just did that)

//2) For each input segment, create two half-edges, and assign their tail vertices and twins.
	TMap<vertex*, TArray<vertex*>> adjacentVertices = GetVertexAdjacencies(vertices, originalVerticies);
	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(adjacentVertices);

	//2a) Do subdivisions here???
	for (int subdivisions = 0; subdivisions < 7; subdivisions++) {
		int midpointCounter = 0;
		TSet<half_edge*> visited = {};
		TArray<vertex*> v1s = {};
		halfEdgesBetweenVertices.GetKeys(v1s);
		for (int i = 0; i < v1s.Num(); i++) {
			vertex* v1 = v1s[i];

			TArray<vertex*> v2s = {};
			halfEdgesBetweenVertices[v1].GetKeys(v2s);
			for (int j = 0; j < v2s.Num(); j++) {
				vertex* v2 = v2s[j];
				if (!visited.Contains(halfEdgesBetweenVertices[v1][v2])) {
					FVector heMidpoint = (v1->location + v2->location) / 2;
					heMidpoint = Util::GetVectorAtDistance(heMidpoint, v1->location.Length());

					ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(heMidpoint));
					newSpherePoint->floatingLabel = "m" + FString::FromInt(midpointCounter);
					newSpherePoint->Initialize();
					midpointCounter++;

					vertex* newVertex = new vertex();
					newVertex->location = heMidpoint;
					newVertex->name = newSpherePoint->floatingLabel;
					vertices.Add(newVertex);

					visited.Add(halfEdgesBetweenVertices[v1][v2]);
					visited.Add(halfEdgesBetweenVertices[v2][v1]);
				}
			}
		}
		adjacentVertices = GetVertexAdjacencies(vertices, originalVerticies);
		halfEdgesBetweenVertices = GetHalfEdgesBetweenVertices(adjacentVertices);
	}

//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	DoClockwiseAssignment(halfEdgesBetweenVertices, originalVerticies);

//6) For every cycle, allocate and assign a face structure.*/
	TArray<face*> faces = GetFacesFromHalfEdges(halfEdgesBetweenVertices);
	UE_LOG(LogTemp, Display, TEXT("faces total = %d"), faces.Num());

//Do subdivisions
	/*for (int subdivisions = 0; subdivisions < 4; subdivisions++) {
		int faceCount = faces.Num();
		for (int f = 0; f < faceCount; f++) {
			face* faceRef = faces[f];

			FVector midpointLoc = FVector(0, 0, 0);
			for (int i = 0; i < faceRef->reps.Num(); i++) {
				midpointLoc += faceRef->reps[i]->tail->location;
			}
			midpointLoc /= faceRef->reps.Num();
			midpointLoc = Util::GetVectorAtDistance(midpointLoc, vertices[0]->location.Length());

			vertex* midpoint = new vertex();
			vertices.Add(midpoint);
			midpoint->location = midpointLoc;
			midpoint->name = "";
			halfEdgesBetweenVertices.Add(midpoint, {});

			TMap<half_edge*, TArray<half_edge*>> from_to_midpointEdgeMap = {};
			for (int e = 0; e < faceRef->reps.Num(); e++) {
				half_edge* edge = faceRef->reps[e];
				midpoint->name += edge->name;

				half_edge* toMid = new half_edge();
				half_edge* fromMid = new half_edge();
				toMid->twin = fromMid;
				fromMid->twin = toMid;
				toMid->tail = edge->tail;
				fromMid->tail = midpoint;

				midpoint->rep = fromMid;

				from_to_midpointEdgeMap.Add(edge, { fromMid, toMid });
			}

			int hec = 0;
			for (int e = 0; e < faceRef->reps.Num(); e++) {
				half_edge* edge = faceRef->reps[e];

				half_edge* new_prev = from_to_midpointEdgeMap[edge][0];
				half_edge* new_next = from_to_midpointEdgeMap[edge->next][1];

				new_prev->next = edge;
				edge->next = new_next;
				new_next->next = new_prev;

				new_prev->prev = new_next;
				edge->prev = new_prev;
				new_next->prev = edge;

				new_prev->name = new_prev->twin->tail->name;
				new_next->name = new_next->twin->tail->name;


				halfEdgesBetweenVertices[new_prev->tail].Add(TTuple<vertex*, half_edge*>(new_prev->twin->tail, new_prev));
				halfEdgesBetweenVertices[new_next->tail].Add(TTuple<vertex*, half_edge*>(new_next->twin->tail, new_next));


				hec += 2;

				UE_LOG(LogTemp, Display, TEXT("added half edge for %s-%s, %s, %s"), *(edge->tail->name), *(edge->twin->tail->name), *(edge->prev->tail->name), *(edge->next->twin->tail->name));
			}
			UE_LOG(LogTemp, Display, TEXT("ddwone with face %s"), *(midpoint->name));
		}
		UE_LOG(LogTemp, Display, TEXT("halfEdgesBetweenVertices num = %d"), halfEdgesBetweenVertices.Num());

		faces = GetFacesFromHalfEdges(halfEdgesBetweenVertices);
		UE_LOG(LogTemp, Display, TEXT("faces total = %d"), faces.Num());

	}*/


//Prepare vertices and triangles
	verticeLocations = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertices[i]->verticesIndex = i;
		verticeLocations.Add(vertices[i]->location);
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
			face* newFace = new face();
			newFace->reps = {};
			newFace->name = "";

			half_edge* current_edge = allHalfEdges[i];
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

void AProceduralGlobe::DoClockwiseAssignment(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices, TSet<vertex*> originalVertices) {
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

