// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpherePoint.generated.h"

UCLASS()
class NETHERCIV_API ASpherePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpherePoint();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int index;

	UFUNCTION(BlueprintImplementableEvent)
	void Initialize();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
