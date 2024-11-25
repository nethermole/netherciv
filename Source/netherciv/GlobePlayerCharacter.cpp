// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobePlayerCharacter.h"

// Sets default values
AGlobePlayerCharacter::AGlobePlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGlobePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGlobePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGlobePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

