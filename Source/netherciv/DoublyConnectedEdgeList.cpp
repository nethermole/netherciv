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
