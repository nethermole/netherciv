// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Person.h"

#include "UnitMove.Generated.h"



/**
 * 
 */
UCLASS(Blueprintable)
class NETHERCIV_API UUnitMove : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	APerson* unitToMove;

	UPROPERTY(BlueprintReadWrite)
	int destinationTileId;

	UUnitMove();
	~UUnitMove();
};
