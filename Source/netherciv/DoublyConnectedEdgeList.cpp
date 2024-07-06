// Fill out your copyright notice in the Description page of Project Settings.


#include "DoublyConnectedEdgeList.h"
#include "Util.h"



DoublyConnectedEdgeList::DoublyConnectedEdgeList()
{
	faces = {};
}

DoublyConnectedEdgeList::~DoublyConnectedEdgeList()
{
}

void DoublyConnectedEdgeList::DoClockwiseAssignment(bool isHexGlobe) {
	//3) For each endpoint, sort the half-edges whose tail vertex is that endpoint in clockwise order.
	//4) For every pair of half-edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin.
	//5) Pick one of the half-edges and assign it as the representative for the endpoint. (Degenerate case: if there's only one half-edge e in the sorted list, set e->twin->next = e and e->prev = e->twin). The next pointers are a permutation on half-edges.

	TArray<vertex*> half_edges_by_v1 = {};
	halfEdgesBetweenVertices.GetKeys(half_edges_by_v1);

	for (int i = 0; i < half_edges_by_v1.Num(); i++) {
		vertex* v1 = half_edges_by_v1[i];
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
			Util::LogVector(expectedNextVector);

			//after this for loop, closestVector is the next clockwise vector
			double closestSoFar = 9999;	//just bigger than 360 i guess
			FVector closestVector = FVector(0, 0, 0);	//it should never remain this, as we already added it above and are now rotating it
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
			Util::LogVector(closestVector);
			clockwiseEdges.Add(half_edges_byTwinTailVector[closestVector]);
		}


		for (int j = 0; j < clockwiseEdges.Num(); j++) {
			UE_LOG(LogTemp, Display, TEXT("\t%s"), *(clockwiseEdges[j]->name));

			half_edge* e1 = clockwiseEdges[j];
			half_edge* e2 = clockwiseEdges[(j + 1) % clockwiseEdges.Num()];	//math to rotate the array to 0 at end

			e1->twin->next = e2;
			e2->prev = e1->twin;
		}

	}
}

TArray<vertex*> DoublyConnectedEdgeList::GenerateHexGlobeVertices() {
	TArray<vertex*> hexGlobeVertices = {};
	TSet<face*> visitedFaces = {};
	for (int i = 0; i < vertices.Num(); i++) {
		vertex* v1 = vertices[i];
		TArray<face*> adjacentFaces = {};

		TArray<half_edge*> halfEdgesFacingAway = {};
		halfEdgesBetweenVertices[v1].GenerateValueArray(halfEdgesFacingAway);
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
	return hexGlobeVertices;
}

void DoublyConnectedEdgeList::Subdivide() {
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

				//ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(heMidpoint));
				//newSpherePoint->floatingLabel = "m" + FString::FromInt(midpointCounter);
				//newSpherePoint->Initialize();
				

				vertex* newVertex = new vertex();
				newVertex->location = heMidpoint;
				newVertex->name = "m" + FString::FromInt(midpointCounter);
				vertices.Add(newVertex);

				midpointCounter++;

				visited.Add(halfEdgesBetweenVertices[v1][v2]);
				visited.Add(halfEdgesBetweenVertices[v2][v1]);
			}
		}
	}
}

void DoublyConnectedEdgeList::LoadIcosahedronCartesianCoordinates() {
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

	vertices = {};
	originalVertices = {};

	TArray<TArray<FVector>> v = {};	//vertex locations

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

					//ASpherePoint* newSpherePoint = GetWorld()->SpawnActor<ASpherePoint>(spherePoint, FTransform(verticeLocation));
					//newSpherePoint->floatingLabel = FString::FromInt(counter);
					

					vertex* newVertex = new vertex();
					newVertex->location = verticeLocation;
					newVertex->name = FString::FromInt(counter);
					counter++;

					vertices.Add(newVertex);
					originalVertices.Add(newVertex);

					//if (matrix == 0) {
					//	newSpherePoint->color = FColor::Red;
					//}
					//if (matrix == 1) {
					//	newSpherePoint->color = FColor::Yellow;
					//}
					//if (matrix == 2) {
					//	newSpherePoint->color = FColor::Blue;
					//}

					//newSpherePoint->Initialize();
				}
			}
		}

	}

	//UE_LOG(LogTemp, Display, TEXT("created %d vertices"), vertices.Num());
}
