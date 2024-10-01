// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "netherciv/util/SpherePoint.h"
#include "netherciv/datastructures/DoublyConnectedEdgeList.h"

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


	void CALCULATEGLOBE_CreateGlobeDcel(int subdivisions);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int faceCount;

	DoublyConnectedEdgeList* dcel;

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetAllVerticeLocations();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllWaterTrianglesBy3s();

	UFUNCTION(BlueprintCallable)
	TArray<FIntVector> GetAllLandTrianglesBy3s();


	DoublyConnectedEdgeList* dcel_property;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
