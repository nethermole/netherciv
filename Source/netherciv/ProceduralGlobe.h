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


	void CreateGlobeDcel(int subdivisions);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int faceCount;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//TSubclassOf<ASpherePoint> spherePoint;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetAllVerticeLocations();

	UFUNCTION(BlueprintCallable)
	TArray<int> GetAllTriangles();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllTrianglesBy3s();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllWaterTrianglesBy3s();
	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllLandTrianglesBy3s();

	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> GetAllUV0();

	TArray<FVector> allVerticeLocations;
	TArray<int> allTriangles;
	TArray<FIntVector> allTrianglesBy3s;
	TArray<FIntVector> waterTriangles;
	TArray<FIntVector> landTriangles;
	TArray<FVector2D> allUV0;


	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetVerticeLocationsByFaceIndex(int index);



	TArray<TArray<FVector>> verticeLocations;

	UFUNCTION(BlueprintCallable)
	TArray<int> GetTrianglesByFaceIndex(int index);

	TArray<TArray<int>> triangles;

	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> getUV0ByFaceIndex(int index);

	TArray<TArray<FVector2D>> uv0s;

	DoublyConnectedEdgeList* dcel;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
