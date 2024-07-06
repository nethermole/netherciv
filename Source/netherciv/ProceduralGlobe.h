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





	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ASpherePoint> spherePoint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> verticeLocations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int> triangles;

	DoublyConnectedEdgeList* dcel;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
