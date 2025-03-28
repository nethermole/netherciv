// Fill out your copyright notice in the Description page of Project Settings.


#include "LoggingFunctionLibrary.h"


void ULoggingFunctionLibrary::Log(FString text)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *text);
}
