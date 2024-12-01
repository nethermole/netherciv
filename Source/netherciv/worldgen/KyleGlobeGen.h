// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "netherciv/datastructures/DoublyConnectedEdgeList.h"

/**
 * 
 */
class NETHERCIV_API KyleGlobeGen
{
public:
	KyleGlobeGen();
	~KyleGlobeGen();
	void OuputGlobeFilesUpToNSubdivisions(int maxSubdivisions);

	void OutputGlobeFile(int subdivisions);

	void CreateGlobeDcel(int subdivisions);

	void LoadGlobeFromFile(int subdivisions);

	TArray<int> GetAdjacentFaceIDs(int faceID);

	TArray<int> GetAdjacentLandFaceIDs(int faceID);

	DoublyConnectedEdgeList* dcel;
};
