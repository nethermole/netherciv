// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "netherciv/datastructures/DoublyConnectedEdgeList.h"
#include "netherciv/worldgen/KyleGlobeGen.h"
#include "netherciv/worldgen/dad/DadGlobeGen.h"

#include "ProceduralGlobe.generated.h"



UCLASS()
class NETHERCIV_API AProceduralGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralGlobe();

	UFUNCTION(BlueprintCallable)
	void GenerateWorld(int subdivisions);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int faceCount;

	DoublyConnectedEdgeList* dcel;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetAllVerticeLocations();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetTriangleIntVectorsForFaceByIndex(int faceIndex);

	UFUNCTION(BlueprintCallable)
	bool isFaceWater(int faceIndex);

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllWaterTrianglesBy3s();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllLandTrianglesBy3s();

	UFUNCTION(BlueprintCallable)
	void GenerateKyleGlobeGenFilesUpToNSubdivisions(int subdivisions);

	UFUNCTION(BlueprintCallable)
	void GenerateDadGlobeGenAtNSubdivisions(int subdivisions);

	DoublyConnectedEdgeList* dcel_property;

	KyleGlobeGen* kyleGlobeGen;
	DadGlobeGen* dadGlobeGen;

	//UFUNCTION(BlueprintCallable)
	//TArray<FIntVector> GetTrianglesBy3sForFace(int faceIndex);

	UFUNCTION(BlueprintCallable)
	int GetFaceCount();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
