// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "netherciv/worldgen/dad/Face.h"
#include "netherciv/worldgen/dad/Vertex.h"

/**
 * 
 */
class NETHERCIV_API DadGlobeGen
{
public:
	DadGlobeGen();
	~DadGlobeGen();

	std::vector<Vertex*> Vertices;
	std::vector<Face*> Faces;

	void Prepare(int subdivisions);

	TArray<FVector> GetAllVerticeLocations();

	static void PrintVerts(std::vector<Vertex*> V);
	static void PrintFaces(std::vector<Face*> F);

private:
	void TwelvePents();
};
