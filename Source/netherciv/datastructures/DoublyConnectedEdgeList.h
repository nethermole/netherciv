// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "set"

struct half_edge {
	struct half_edge* prev;  /* prev->next == this */
	struct half_edge* next;  /* next->prev == this */
	struct half_edge* twin;  /* twin->twin == this */
	struct vertex* tail;     /* twin->next->tail == tail &&
								prev->twin->tail == tail */
	struct face* left;       /* prev->left == left && next->left == left */
	FString name;
};

struct face {
	TArray<half_edge*> reps;

	FString name;
};

struct vertex {
	struct half_edge* rep;  /* rep->tail == this */		//we are only tracking one half edge per vertex, but n vertices may have this vertex as its tail
	FVector location;
	FString name;

	int verticesIndex;
};


class VertexAdjacenciesComparator {
public:
	bool operator()(std::pair<vertex*, double> v1, std::pair<vertex*, double> v2) {
		return v1.second < v2.second;
	}
};


/**
 * 
 */
class NETHERCIV_API DoublyConnectedEdgeList
{
public:
	DoublyConnectedEdgeList();
	~DoublyConnectedEdgeList();

	DoublyConnectedEdgeList* CreateGoldbergPolyhedronFromSubdividedIcosahedron();

	void LoadIcosahedronCartesianCoordinates();
	void Subdivide();
	TArray<vertex*> GenerateHexGlobeVertices();
	void DoClockwiseAssignment(bool isHexGlobe);

	void GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices_param);

	static TArray<half_edge*>  CreateHalfEdges(vertex* v1, vertex* v2);


	TMap<vertex*, TArray<vertex*>> GetVertexAdjacencies(TArray<vertex*> vertices_param, bool isHexGlobe);
	TMap<vertex*, TMap<vertex*, half_edge*>> GetHalfEdgesBetweenVertices(TMap<vertex*, TArray<vertex*>> adjacentVertices_param);

	void CalculateHalfEdges(bool isHexGlobe);

	void PrepareVerticeLocationsAndTriangles();

	TArray<vertex*> vertices;
	TSet<vertex*> originalVertices;

	TArray<face*> faces;

	TMap<vertex*, TArray<vertex*>> adjacentVertices;
	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices;

	TArray<FIntVector> trianglesBy3s;

	TArray<FIntVector> waterTrianglesBy3s;
	TArray<FIntVector> landTrianglesBy3s;

	TArray<FVector> allVerticeLocations;
	TArray<int> allTriangles;

	static bool IsTriangle(face* face_in);
	static bool IsPentagon(face* face_in);
	static bool IsHexagon(face* face_in);

	TMap<vertex*, std::priority_queue<std::pair<vertex*, double>, std::vector<std::pair<vertex*, double>>, VertexAdjacenciesComparator>> newApproachCache;
	std::set<std::pair<vertex*, vertex*>> alreadyVisisted;

	void WriteToFile(int subd);
	void ReadFromFile(int subd);

private:
	int UE_DIST_GLOBE_RADIUS;
};
