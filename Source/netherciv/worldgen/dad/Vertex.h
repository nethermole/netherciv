// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NETHERCIV_API Vertex
{
public:
	Vertex(double x, double y, double z);
	Vertex();
	~Vertex();
	
	static int Nverts;

	double xLoc;
	double yLoc;
	double zLoc;

	void Middle(Vertex* P1, double X, double Y, double Z);

};
