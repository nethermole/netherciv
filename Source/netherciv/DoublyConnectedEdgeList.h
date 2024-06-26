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
	~DoublyConnectedEdgeList();


	TArray<face*> faces;
};
