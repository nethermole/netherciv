// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "netherciv/worldgen/dad/Vertex.h"

/**
 * 
 */
class NETHERCIV_API Face
{
public:
	Face(int N);
	Face(int P1, int P2, int P3, int P4, int P5, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5);
	Face(int P1, int P2, int P3, int P4, int P5, int P6, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5, int Adj6);
	~Face();

	void SetVerts(int P1, int P2, int P3, int P4, int P5, int P6, int Adj1, int Adj2, int Adj3, int Adj4, int Adj5, int Adj6);
	void ClearFlags();
	void Divide(std::vector<Vertex*> &Vertices, std::vector<Face*> &Faces);

	int index; int edges;

	std::vector<int> Verts; std::vector<int> Adjacent;
	std::vector<int> Flags;

	static int Nfaces;

	double NormX; double NormY; double NormZ;
};
