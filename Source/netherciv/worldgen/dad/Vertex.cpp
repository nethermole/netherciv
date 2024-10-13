// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/dad/Vertex.h"

int Vertex::Nverts = 0;

Vertex::Vertex(double x, double y, double z)
{
	double L = sqrt(x * x + y * y + z * z);
	xLoc = x / L;
	yLoc = y / L;
	zLoc = z / L;
	Vertex::Nverts = Vertex::Nverts + 1;
}

FVector Vertex::ToVector() {
	return FVector(xLoc, yLoc, zLoc);
}

Vertex::Vertex()
{
	xLoc = 0;
	yLoc = 0;
	zLoc = 0;
	Vertex::Nverts = Vertex::Nverts + 1;
}

Vertex::~Vertex()
{
}

void Vertex::Middle(Vertex* P1, double X, double Y, double Z)
{
	xLoc = (P1->xLoc + X) / 2.;
	yLoc = (P1->yLoc + Y) / 2.;
	zLoc = (P1->zLoc + Z) / 2.;
	double L = sqrt(xLoc * xLoc + yLoc * yLoc + zLoc * zLoc);
	xLoc = xLoc / L;
	yLoc = yLoc / L;
	zLoc = zLoc / L;
}
