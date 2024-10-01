// Fill out your copyright notice in the Description page of Project Settings.


#include "SpherePoint.h"

// Sets default values
ASpherePoint::ASpherePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpherePoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpherePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

