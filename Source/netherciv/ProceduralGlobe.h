// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SpherePoint.h"
#include "DoublyConnectedEdgeList.h"

#include "ProceduralGlobe.generated.h"



UCLASS()
class NETHERCIV_API AProceduralGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralGlobe();

	UFUNCTION(BlueprintCallable)
	void GenerateWorld();

	TMap<vertex*, TArray<vertex*>> GetVertexAdjacencies(TArray<vertex*> vertices);

	TArray<half_edge*>  CreateHalfEdges(vertex* v1, vertex* v2);

	TMap<vertex*, TMap<vertex*, half_edge*>> GetHalfEdgesBetweenVertices(TMap<vertex*, TArray<vertex*>> adjacentVertices);

	void DoClockwiseAssignment(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices);

	TArray<face*> GetFacesFromHalfEdges(TMap<vertex*, TMap<vertex*, half_edge*>> halfEdgesBetweenVertices);

	void LogVector(FVector in);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ASpherePoint> spherePoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> verticeLocations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int> triangles;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
