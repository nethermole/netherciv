// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "netherciv/datastructures/DoublyConnectedEdgeList.h"
#include <netherciv/zlib-1.3.1/zlib.h>

/**
 * 
 */
class NETHERCIV_API KyleGlobeGen
{
public:
	KyleGlobeGen();
	~KyleGlobeGen();
	void OuputGlobeFilesUpToNSubdivisions(int maxSubdivisions);

	void OutputGlobeFile(int subdivisions);

	void CreateGlobeDcel(int subdivisions);

	void LoadGlobeFromFile(int subdivisions);

	TArray<int> GetAdjacentFaceIDs(int faceID);

	TArray<int> GetAdjacentLandFaceIDs(int faceID);

	int CompressIntArray(TArray<int> start, std::vector<uint8_t>& compressedOutput);

	std::vector<int> InflateIntArray(int numInflatedBytes, std::vector<uint8_t>& compressedInput);

	void compressStringAndDecompress();

	int decompress_vector(uLongf destination_length, std::vector<uint8_t> source, std::vector<wchar_t>& destination);

	int decompress_vector_int(uLongf destinationAllocationSize, std::vector<uint8_t> source, std::vector<int>& destination);

	void add_buffer_to_vector(std::vector<wchar_t>& vector, const Bytef* buffer, uLongf length);

	void add_buffer_to_vector_int(std::vector<int>& vector, const Bytef* buffer, uLongf length);

	void compress_memory(void* in_data, size_t in_data_size, std::vector<uint8_t>& out_data);

	TArray<int> EncodeEntireMap();

	DoublyConnectedEdgeList* dcel;
};
