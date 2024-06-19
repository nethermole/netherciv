// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "PentGlobeTile.h"

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
	void DrawBoard();
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APentGlobeTile> pentGlobeTile;

};
