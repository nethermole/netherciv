// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PentGlobeTile.h"
#include "HexGlobeTile.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlobeMaker.generated.h"

UCLASS()
class NETHERCIV_API AGlobeMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGlobeMaker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void DrawBoard(double sphereX, double sphereY, double sphereZ, int hexesBetweenPents);
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APentGlobeTile> pentGlobeTile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<AHexGlobeTile> hexGlobeTile;

private:
	double flatRadiusHex;
	double edgeLength;
	double centerPentagonAngle;
	double centerHexagonAngle;
	double flatRadiusPent;
	double outerDihedralAngle;

	bool IsHex(AActor* actor);
	bool IsPent(AActor* actor);

	void CalculateAttributes(int hexesBetweenPents);
	TArray<AHexGlobeTile*> CreateInitialLongitudeHexes(APentGlobeTile* northPoleTile, int hexesBetweenPents, double& result_totalDx, double& result_totalDz);
	TArray<AHexGlobeTile*> FillInTriangleHexes(TArray<AHexGlobeTile*> longitudeHexes);
	APentGlobeTile* CreateRow1Pent(double longitudeHexesDx, double longitudeHexesDz, int hexesBetweenPents);
	TArray<AHexGlobeTile*> RotateInitialLongitudeHexesAroundRow1Pent(TArray<AHexGlobeTile*> longitudeHexes, APentGlobeTile* row1PentTile);
	TArray<AHexGlobeTile*> RotateInitialLongitudeHexesAroundRow1PentToEquator(TArray<AHexGlobeTile*> longitudeHexes, APentGlobeTile* row1PentTile);
	TArray<AActor*> RotateNorthernHemisphereFifth(TArray<AActor*> northernHemisphereTilesToRotate);
	TArray<AActor*> RotateToSouthHemisphere(TArray<AActor*> northTiles);
};
