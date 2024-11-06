// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "netherciv/bitmap/BMPImage.h"

#include "netherciv/worldgen/dad/Face.h"
#include "netherciv/worldgen/dad/Vertex.h"

/**
 * FUNCTIONAL, BUT HAS MEMORY LEAK. FIX MEMORY LEAK TO USE
 */
class NETHERCIV_API DadGlobeGen
{
public:
	DadGlobeGen();
	~DadGlobeGen();

	bool isWater(BMPImage& globeImage, FVector midpoint);

	std::vector<Vertex*> Vertices;
	std::vector<Face*> Faces;

	void Prepare(int subdivisions);
	void CalculateTrianglesAndCalculateWaterAndLand();

	TArray<FIntVector> GetWaterTrianglesBy3s();
	TArray<FIntVector> GetLandTrianglesBy3s();

	TArray<FVector> GetAllVerticeLocations();
	TArray<FVector> allVerticeLocations;

	static void PrintVerts(std::vector<Vertex*> V);
	static void PrintFaces(std::vector<Face*> F);

private:
	void TwelvePents();

	TArray<FIntVector> waterTrianglesBy3s;
	TArray<FIntVector> landTrianglesBy3s;

	int UE_DIST_GLOBE_RADIUS;
};
