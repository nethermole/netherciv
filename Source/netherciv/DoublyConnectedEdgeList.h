// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	struct half_edge* rep;  /* rep->left == this */
	struct half_edge* rep2;  /* rep->left == this */
	struct half_edge* rep3;  /* rep->left == this */
	struct half_edge* rep4;  /* rep->left == this */
	struct half_edge* rep5;  /* rep->left == this */

	FString name;
};

struct vertex {
	struct half_edge* rep;  /* rep->tail == this */
	FVector location;
	FString name;

	int verticesIndex;
};



/**
 * 
 */
class NETHERCIV_API DoublyConnectedEdgeList
{
public:
	DoublyConnectedEdgeList();
	DoublyConnectedEdgeList(TArray<vertex*> vertices_in);
	~DoublyConnectedEdgeList();

	void LoadIcosahedronCartesianCoordinates();
	void Subdivide();
	TArray<vertex*> GenerateHexGlobeVertices();
	void DoClockwiseAssignment(bool isHexGlobe);

	TMap<vertex*, TArray<vertex*>> GetHexGlobeAdjacencies(TArray<vertex*> hexGlobeVertices_param);

	void GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices_param);

	static TArray<half_edge*>  CreateHalfEdges(vertex* v1, vertex* v2);


	TMap<vertex*, TArray<vertex*>> GetVertexAdjacencies(TArray<vertex*> vertices_param);
	TMap<vertex*, TMap<vertex*, half_edge*>> GetHalfEdgesBetweenVertices(TMap<vertex*, TArray<vertex*>> adjacentVertices_param);

	void CalculateHalfEdges();

	void PrepareVerticeLocationsAndTriangles();

	TArray<vertex*> vertices;
	TSet<vertex*> originalVertices;

	TArray<face*> faces;

	TMap<vertex*, TArray<vertex*>> adjacentVertices;
	TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices;

	TMap<vertex*, TArray<vertex*>> hexGlobeAdjacencies;
	TArray<vertex*> hexGlobeVertices;

	TArray<FVector> verticeLocations;
	TArray<int> triangles;
};
