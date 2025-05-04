// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class NETHERCIV_API ZLIBWrapper
{
public:
	ZLIBWrapper();
	~ZLIBWrapper();

	void InflateMemory(std::vector<uint8_t> source, std::vector<uint8_t>& destination, int expectedDestinationLength);

	void CompressMemory(void* in_data, size_t in_data_size, std::vector<uint8_t>& out_data);

};
