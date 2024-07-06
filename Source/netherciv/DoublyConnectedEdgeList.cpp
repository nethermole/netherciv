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
	originalVerticies = {};

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
					originalVerticies.Add(newVertex);

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
