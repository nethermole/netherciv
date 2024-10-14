// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/dad/DadGlobeGen.h"
#include "netherciv/util/Util.h"

#include "cmath"
#include "chrono"

#include "Kismet/GameplayStatics.h"


DadGlobeGen::DadGlobeGen()
{
	UE_DIST_GLOBE_RADIUS = 1000;
}

DadGlobeGen::~DadGlobeGen()
{
}

bool DadGlobeGen::isWater(BMPImage& globeImage, FVector midpoint) {
	//Start determining if water
	float atanX = atan(midpoint.Y / midpoint.X);
	float radiansAroundGlobe;
	//UE_LOG(LogTemp, Display, TEXT("x:%f, y:%f, atan:%f"), midpoint.X, midpoint.Y, FMath::RadiansToDegrees(atanX));
	if (midpoint.Y > 0 && midpoint.X > 0) {
		radiansAroundGlobe = atanX;
	}
	else if (midpoint.Y > 0 && midpoint.X < 0) {
		radiansAroundGlobe = UE_PI + atanX;
	}
	else if (midpoint.Y < 0 && midpoint.X < 0) {
		radiansAroundGlobe = UE_PI + atanX;
	}
	else if (midpoint.Y < 0 && midpoint.X > 0) {
		radiansAroundGlobe = (2 * UE_PI) + atanX;
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("How this happen? x:%f, y:%f"), midpoint.X, midpoint.Y);
		return false;
	}
	float percentRadiallyAroundGlobe = radiansAroundGlobe / (2 * UE_PI);
	//UE_LOG(LogTemp, Display, TEXT("x:%f, y:%f, degreesAroundGlobe:%f, percentRadiallyAroundGlobe:%f"), midpoint.X, midpoint.Y, FMath::RadiansToDegrees(radiansAroundGlobe), percentRadiallyAroundGlobe);
	int pixelX = globeImage.getWidth() * (1.0f - percentRadiallyAroundGlobe);	//gotta invert x-axis

	float percentLinearlyUpGlobe = (midpoint.Z + UE_DIST_GLOBE_RADIUS) / (UE_DIST_GLOBE_RADIUS * 2);	//weird math because the southern hemisphere has negative Z-coord
	int pixelY = globeImage.getHeight() * percentLinearlyUpGlobe;

	//UE_LOG(LogTemp, Display, TEXT("Z:%f, percZ:%f"), midpoint.Z, percentLinearlyUpGlobe);

	Color mapPointHexColor = globeImage.GetColor(pixelX, pixelY);
	bool water = mapPointHexColor.r > 0.95;
	//End getting water
	return water;
}

void DadGlobeGen::CalculateTrianglesAndCalculateWaterAndLand()
{
	UE_LOG(LogTemp, Display, TEXT("Calculating vertices"));

	waterTrianglesBy3s = {};
	landTrianglesBy3s = {};

	allVerticeLocations = {};
	allVerticeLocations.Add(FVector());

	for (int i = 1; i < Vertices.size() && (Vertices[i] != NULL); i++) {
		if (i % 10000 == 0) {
			UE_LOG(LogTemp, Display, TEXT("vertice %d"), i);
		}

		allVerticeLocations.Add(
			Util::GetVectorAtDistance(
				FVector(
					Vertices[i]->xLoc,
					Vertices[i]->yLoc,
					Vertices[i]->zLoc
				)
				, UE_DIST_GLOBE_RADIUS)
		);
	}

	UE_LOG(LogTemp, Display, TEXT("Calculating land"));

	BMPImage globeImage(0, 0);	//you can make a default constructor...
	globeImage.Read("C:/temp/subd/equirectangularProjection_cropped.bmp");

	for (int j = 1; j < Faces.size() && (Faces[j] != NULL); j++){
		if (j % 10000 == 0) {
			UE_LOG(LogTemp, Display, TEXT("face %d"), j);
		}

		Face* face = Faces[j];

		FVector midpoint = FVector(0, 0, 0);
		for (int i = 1; i <= face->edges; i++) {
			midpoint += allVerticeLocations[face->Verts[i]];
		}
		midpoint /= face->edges;
		allVerticeLocations.Add(midpoint);

		bool isWater = this->isWater(globeImage, midpoint);
		for (int i = 1; i <= face->edges; i++) {
			int nextVert = i + 1;
			if (nextVert > face->edges) {
				nextVert = 1;
			}

			if (isWater) {
				waterTrianglesBy3s.Add(
					FIntVector(
						allVerticeLocations.Num() - 1,
						face->Verts[i],
						face->Verts[nextVert]
					)
				);
			}
			else {
				landTrianglesBy3s.Add(
					FIntVector(
						allVerticeLocations.Num() - 1,
						face->Verts[i],
						face->Verts[nextVert]
					)
				);
			}

		}
	}
}

TArray<FIntVector> DadGlobeGen::GetWaterTrianglesBy3s()
{
	return waterTrianglesBy3s;
}

TArray<FIntVector> DadGlobeGen::GetLandTrianglesBy3s()
{
	return landTrianglesBy3s;
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


	//PrintVerts(Vertices);
	//PrintFaces(Faces);
	UE_LOG(LogTemp, Display, TEXT("DadGlobeGen - Subdivisions(%d) took %d seconds. Now calculating triangles, land/water..."), subdivisions, static_cast<int>(FPlatformTime::Seconds()-start));

	CalculateTrianglesAndCalculateWaterAndLand();
	UE_LOG(LogTemp, Display, TEXT("DadGlobeGen - finished"));
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

TArray<FVector> DadGlobeGen::GetAllVerticeLocations() {
	return allVerticeLocations;
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
	UE_LOG(LogTemp, Display, TEXT("\nVertex and Adjacent face indices for %d faces:"), F[1]->Nfaces);
	for (int i = 1; i <= F[1]->Nfaces; i++) {
		Face* face = F[i];

		for (int edgeIndex = 0; edgeIndex < face->edges; edgeIndex++) {
			if (face->edges == 5) {
				UE_LOG(LogTemp, Display, TEXT("%d - %d %d %d %d %d   - %d %d %d %d %d  "),
					i,
					face->Verts[1],
					face->Verts[2],
					face->Verts[3],
					face->Verts[4],
					face->Verts[5],
					face->Adjacent[1],
					face->Adjacent[2],
					face->Adjacent[3],
					face->Adjacent[4],
					face->Adjacent[5]
				);
			}
			else if (face->edges == 6) {
				UE_LOG(LogTemp, Display, TEXT("%d - %d %d %d %d %d %d - %d %d %d %d %d %d"),
					i,
					face->Verts[1],
					face->Verts[2],
					face->Verts[3],
					face->Verts[4],
					face->Verts[5],
					face->Verts[6],
					face->Adjacent[1],
					face->Adjacent[2],
					face->Adjacent[3],
					face->Adjacent[4],
					face->Adjacent[5],
					face->Adjacent[6]
				);
			}
		}


		//UE_LOG(LogTemp, Display, TEXT("%7d, %2d; "), i, F[i]->edges);
		//for (j = 1; j <= 6; j++) {
		//	if (F[i]->Verts[j] > 0) { UE_LOG(LogTemp, Display, TEXT("%5d,"), F[i]->Verts[j]); }
		//	else { UE_LOG(LogTemp, Display, TEXT("   - ,")); }
		//}
		//UE_LOG(LogTemp, Display, TEXT("   "));
		//for (j = 1; j <= 6; j++) {
		//	if (F[i]->Adjacent[j] > 0) { UE_LOG(LogTemp, Display, TEXT("%5d,"), F[i]->Adjacent[j]); }
		//	else { UE_LOG(LogTemp, Display, TEXT("   - ,")); }
		//}

		//UE_LOG(LogTemp, Display, TEXT("   "));
		//for (j = 1; j <= 6; j++) {
		//	UE_LOG(LogTemp, Display, TEXT("%2d,"), F[i]->Flags[j]);
		//}
		//UE_LOG(LogTemp, Display, TEXT("   "));
		//UE_LOG(LogTemp, Display, TEXT("%6.3f, %6.3f, %6.3f"), F[i]->NormX, F[i]->NormY, F[i]->NormZ);

		//UE_LOG(LogTemp, Display, TEXT(""));
	}
}