// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/dad/DadGlobeGen.h"

#include "cmath"
#include "chrono"

#include "Kismet/GameplayStatics.h"

DadGlobeGen::DadGlobeGen()
{
}

DadGlobeGen::~DadGlobeGen()
{
}

void DadGlobeGen::Prepare(int subdivisions)
{
	UE_LOG(LogTemp, Display, TEXT("DadGlobeGen - Prepare() started"));

	float start = FPlatformTime::Seconds();

	Face::Nfaces = 0;
	Vertex::Nverts = 0;

	int Nfaces = 0;
	int i, N;

	//int verticesLength = (20 * (int)(pow(4.0, 1.01 * subdivisions) + 1) + 1);
	//UE_LOG(LogTemp, Display, TEXT("Vertices: %d"), verticesLength);

	Vertices = std::vector<Vertex*>(1000000000);
	Faces = std::vector<Face*>(500000000);

	TwelvePents();

	for (N = 1; N <= subdivisions; N++) {
		Nfaces = Faces[1]->Nfaces;
		for (i = 1; i <= Nfaces; i++) {
			Faces[i]->ClearFlags();
		}
		for (i = 1; i <= Nfaces; i++) {
			Faces[i]->Divide(Vertices, Faces);
		}
	}


	PrintVerts(Vertices);
	PrintFaces(Faces);
	UE_LOG(LogTemp, Display, TEXT("DadGlobeGen - Subdivisions(%d) took %d seconds"), subdivisions, static_cast<int>(FPlatformTime::Seconds()-start));
}

void DadGlobeGen::TwelvePents()
{
	double GR = (1 + sqrt(5.0)) / 2;

	Vertices[12] = new Vertex(-1., -1., -1.);
	Vertices[20] = new Vertex(-1., -1., 1.);
	Vertices[10] = new Vertex(-1., 1., -1.);
	Vertices[16] = new Vertex(-1., 1., 1.);
	Vertices[6] = new Vertex(1., -1., -1.);
	Vertices[14] = new Vertex(1., -1., 1.);
	Vertices[1] = new Vertex(1., 1., -1.);
	Vertices[8] = new Vertex(1., 1., 1.);
	Vertices[5] = new Vertex(0., -1. / GR, -1. * GR);
	Vertices[19] = new Vertex(0., -1. / GR, GR);
	Vertices[4] = new Vertex(0., 1. / GR, -1. * GR);
	Vertices[15] = new Vertex(0., 1. / GR, GR);
	Vertices[18] = new Vertex(-1. / GR, -1. * GR, 0.);
	Vertices[9] = new Vertex(-1. / GR, 1. * GR, 0.);
	Vertices[13] = new Vertex(1. / GR, -1. * GR, 0.);
	Vertices[3] = new Vertex(1. / GR, 1. * GR, 0.);
	Vertices[11] = new Vertex(-1. * GR, 0., -1. / GR);
	Vertices[17] = new Vertex(-1. * GR, 0., 1. / GR);
	Vertices[2] = new Vertex(1. * GR, 0., -1. / GR);
	Vertices[7] = new Vertex(1. * GR, 0., 1. / GR);

	Faces[1] = new Face(1, 2, 7, 8, 3, 3, 4, 8, 5, 2);
	Faces[2] = new Face(1, 3, 9, 10, 4, 1, 5, 9, 6, 3);
	Faces[3] = new Face(1, 4, 5, 6, 2, 2, 6, 7, 4, 1);
	Faces[4] = new Face(2, 6, 13, 14, 7, 3, 7, 11, 8, 1);
	Faces[5] = new Face(3, 8, 15, 16, 9, 1, 8, 12, 9, 2);
	Faces[6] = new Face(4, 10, 11, 12, 5, 2, 9, 10, 7, 3);
	Faces[7] = new Face(5, 12, 18, 13, 6, 6, 10, 11, 4, 3);
	Faces[8] = new Face(7, 14, 19, 15, 8, 4, 11, 12, 5, 1);
	Faces[9] = new Face(9, 16, 17, 11, 10, 5, 12, 10, 6, 2);
	Faces[10] = new Face(11, 17, 20, 18, 12, 9, 12, 11, 7, 6);
	Faces[11] = new Face(13, 18, 20, 19, 14, 7, 10, 12, 8, 4);
	Faces[12] = new Face(15, 19, 20, 17, 16, 8, 11, 10, 9, 5);
}

void DadGlobeGen::PrintVerts(std::vector<Vertex*> V)
{
	int i;
	UE_LOG(LogTemp, Display, TEXT("Coordinates for %d Vertices"), V[1]->Nverts);
	for (i = 1; i <= V[1]->Nverts; i++) {
		UE_LOG(LogTemp, Display, TEXT("%d; %f, %f, %f"), i, V[i]->xLoc, V[i]->yLoc, V[i]->zLoc);
	}
}

void DadGlobeGen::PrintFaces(std::vector<Face*> F)
{
	int i, j;
	UE_LOG(LogTemp, Display, TEXT("Vertex and adjacent side indices for %d faces:\nindex, adjacentVertexCount, adjacentVertices, adjacentEdges"), F[1]->Nfaces);
	for (i = 1; i <= F[1]->Nfaces; i++) {
		UE_LOG(LogTemp, Display, TEXT("%7d, %2d; "), i, F[i]->edges);
		for (j = 1; j <= 6; j++) {
			if (F[i]->Verts[j] > 0) { UE_LOG(LogTemp, Display, TEXT("%5d,"), F[i]->Verts[j]); }
			else { UE_LOG(LogTemp, Display, TEXT("   - ,")); }
		}
		UE_LOG(LogTemp, Display, TEXT("   "));
		for (j = 1; j <= 6; j++) {
			if (F[i]->Adjacent[j] > 0) { UE_LOG(LogTemp, Display, TEXT("%5d,"), F[i]->Adjacent[j]); }
			else { UE_LOG(LogTemp, Display, TEXT("   - ,")); }
		}

		UE_LOG(LogTemp, Display, TEXT("   "));
		for (j = 1; j <= 6; j++) {
			UE_LOG(LogTemp, Display, TEXT("%2d,"), F[i]->Flags[j]);
		}
		UE_LOG(LogTemp, Display, TEXT("   "));
		UE_LOG(LogTemp, Display, TEXT("%6.3f, %6.3f, %6.3f"), F[i]->NormX, F[i]->NormY, F[i]->NormZ);

		UE_LOG(LogTemp, Display, TEXT(""));
	}
}