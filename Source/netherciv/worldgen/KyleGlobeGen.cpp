// Fill out your copyright notice in the Description page of Project Settings.


#include "netherciv/worldgen/KyleGlobeGen.h"
#include <cassert>


KyleGlobeGen::KyleGlobeGen()
{
}

KyleGlobeGen::~KyleGlobeGen()
{
}

void KyleGlobeGen::OuputGlobeFilesUpToNSubdivisions(int maxSubdivisions) {
	for (int i = 0; i <= maxSubdivisions; i++) {
		OutputGlobeFile(i);
	}
}

void KyleGlobeGen::OutputGlobeFile(int subdivisions) {
		CreateGlobeDcel(subdivisions);
		dcel->WriteToFile(subdivisions);
}

//1) For each endpoint, create a vertex.
//2)For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
//3) Subdivide n times
	//3a) Generate new vertices
	//3b) For each input segment, create two half-edges, and assign their tail vertices and twins. Pick one of the half-edges and assign it as the representative for the endpoints.
//4)
	//4a) For each endpoint, sort the half - edges whose tail vertex is that endpoint in clockwise order.
	//4b) For every pair of half - edges e1, e2 in clockwise order, assign e1->twin->next = e2 and e2->prev = e1->twin
//5)For every cycle, allocate and assign a face structure.
void KyleGlobeGen::CreateGlobeDcel(int subdivisions)
{
	TRACE_BOOKMARK(TEXT("CreateGlobeDcel bookmark"))
	TRACE_CPUPROFILER_EVENT_SCOPE(AProceduralGlobe::CALCULATEGLOBE_CreateGlobeDcel)
	//Load the subdivided icosahedron
	dcel = new DoublyConnectedEdgeList();

	dcel->LoadIcosahedronCartesianCoordinates();	//1
	dcel->CalculateHalfEdges(false);		//2

	for (int subdivCount = 0; subdivCount < subdivisions; subdivCount++) {	//3
		dcel->Subdivide();
		dcel->CalculateHalfEdges(false);
	}
	dcel->DoClockwiseAssignment(false);		//4
	dcel->GetFacesFromHalfEdges(dcel->halfEdgesBetweenVertices);	//5


	//To hexes
	DoublyConnectedEdgeList* hexDcel = dcel->CreateGoldbergPolyhedronFromSubdividedIcosahedron();
	hexDcel->PrepareVerticeLocationsAndTriangles();
	dcel = hexDcel;
}

void KyleGlobeGen::LoadGlobeFromFile(int subdivisions)
{
	dcel = new DoublyConnectedEdgeList();
	if (subdivisions > 8) {
		UE_LOG(LogTemp, Display, TEXT("Cannot generate world with more than 8 subdivisions yet"));
		return;
	}
	dcel->ReadFromFile(subdivisions);
	dcel->PrepareVerticeLocationsAndTriangles();
}

TArray<int> KyleGlobeGen::GetAdjacentFaceIDs(int faceID)
{
	UE_LOG(LogTemp, Display, TEXT("Getting faceIDs adjacent to %d"), faceID);

	TArray<int> adjacentFaceIDs = {};
	for (half_edge* faceEdge : dcel->faces[faceID]->reps) {
		adjacentFaceIDs.Add(faceEdge->twin->faceRef->faceIndex);
	}
	
	return adjacentFaceIDs;
}

TArray<int> KyleGlobeGen::GetAdjacentLandFaceIDs(int faceID)
{
	TArray<int> adjacentFaces = GetAdjacentFaceIDs(faceID);
	adjacentFaces.RemoveAll([this](const int& val) {return dcel->faces[val]->isWater; });

	return adjacentFaces;
}

int KyleGlobeGen::CompressIntArray(TArray<int> start, std::vector<uint8_t>& compressedOutput) {
	int startAllocatedMemory = start.Num() * start.GetTypeSize();

	void* startDataPointer = start.GetData();

	compress_memory(startDataPointer, startAllocatedMemory, compressedOutput);

	UE_LOG(LogTemp, Display, TEXT("Compressed array of int with count %d to array of unsigned char with count %d"), start.Num(), compressedOutput.size());

	return startAllocatedMemory;
}

std::vector<int> KyleGlobeGen::InflateIntArray(int numInflatedBytes, std::vector<uint8_t>& compressedInput) {

	std::vector<int> inflated = {};
	decompress_vector_int((uLongf)numInflatedBytes, compressedInput, inflated);
	UE_LOG(LogTemp, Display, TEXT("Decompressed array of int with count %d"), inflated.size());

	return inflated;
}

void KyleGlobeGen::compressStringAndDecompress() {

	//Initialize
	FString start = "ooooooooooooooooooaaaaaaaaaaaaaaaaaaaaacbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbasdfnasfdaeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";
	UE_LOG(LogTemp, Display, TEXT("Start string: %s"), *start);
	UE_LOG(LogTemp, Display, TEXT("Start string length: %d"), start.Len());

	TArray<wchar_t> start_wchar_tTArray = start.GetCharArray();
	int startAllocatedMemory = start_wchar_tTArray.Num() * start_wchar_tTArray.GetTypeSize();
	UE_LOG(LogTemp, Display, TEXT("Start char array allocated memory: %d"), startAllocatedMemory);



	//Compress
	void* startDataPointer = start_wchar_tTArray.GetData();
	std::vector<uint8_t> compressed = {};

	UE_LOG(LogTemp, Display, TEXT("Begin - Memory Compress"));
	compress_memory(startDataPointer, startAllocatedMemory, compressed);
	UE_LOG(LogTemp, Display, TEXT("Complete - Memory Compress"));

	UE_LOG(LogTemp, Display, TEXT("Compressed memory array length: %d"), compressed.size());


	//Inflate
	std::vector<wchar_t> inflated;
	UE_LOG(LogTemp, Display, TEXT("Begin - Memory Inflate"));
	decompress_vector((uLongf)startAllocatedMemory, compressed, inflated);
	UE_LOG(LogTemp, Display, TEXT("End - Memory Inflate"));


	FString newString = *(new FString());
	for (int i = 0; i < inflated.size(); i++) {
		int n = inflated[i];
		UE_LOG(LogTemp, Display, TEXT("%d:  %d"), i, n);

		newString = newString.AppendChar(inflated[i]);
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *newString);
}


int KyleGlobeGen::decompress_vector(uLongf destinationAllocationSize, std::vector<uint8_t> source, std::vector<wchar_t>& destination) {
	Bytef* destination_data = (Bytef*) malloc(destinationAllocationSize);
	if (destination_data == nullptr) {
		return Z_MEM_ERROR;
	}

	int return_value = uncompress(destination_data, &destinationAllocationSize, (Bytef*) source.data(), source.size());
	add_buffer_to_vector(destination, destination_data, destinationAllocationSize);
	free(destination_data);
	return return_value;
}

int KyleGlobeGen::decompress_vector_int(uLongf destinationAllocationSize, std::vector<uint8_t> source, std::vector<int>& destination) {
	Bytef* destination_data = (Bytef*)malloc(destinationAllocationSize);
	if (destination_data == nullptr) {
		return Z_MEM_ERROR;
	}

	int return_value = uncompress(destination_data, &destinationAllocationSize, (Bytef*)source.data(), source.size());
	add_buffer_to_vector_int(destination, destination_data, destinationAllocationSize);
	free(destination_data);
	return return_value;
}

void KyleGlobeGen::add_buffer_to_vector(std::vector<wchar_t>& vector, const Bytef* buffer, uLongf length) {
	for (uLongf byte_index = 0; byte_index < length; byte_index++) {
		char current_character = buffer[byte_index];
		vector.push_back(current_character);
	}
}

void KyleGlobeGen::add_buffer_to_vector_int(std::vector<int>& vector, const Bytef* buffer, uLongf length) {
	for (uLongf byte_index = 0; byte_index < length; byte_index+=sizeof(int)) {
		int current_character = buffer[byte_index] ;
		vector.push_back(current_character);
	}
}

void KyleGlobeGen::compress_memory(void* in_data, size_t in_data_size, std::vector<uint8_t>& out_data)
{
	std::vector<uint8_t> buffer;

	const size_t BUFSIZE = 128 * 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = reinterpret_cast<uint8_t*>(in_data);
	strm.avail_in = in_data_size;
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	deflateInit(&strm, Z_BEST_COMPRESSION);

	while (strm.avail_in != 0)
	{
		int res = deflate(&strm, Z_NO_FLUSH);
		assert(res == Z_OK);
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
	}

	int deflate_res = Z_OK;
	while (deflate_res == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;
		}
		deflate_res = deflate(&strm, Z_FINISH);
	}

	assert(deflate_res == Z_STREAM_END);
	buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	out_data.swap(buffer);
}


TArray<int> KyleGlobeGen::EncodeEntireMap()
{
	TArray<int> facesWith0Water1Land = {};

	for (int i = 0; i < dcel->faces.Num(); i++) {
		if (dcel->faces[i]->isWater) {
			facesWith0Water1Land.Add(0);
		}
		else {
			facesWith0Water1Land.Add(1);
		}
	}

	std::vector<uint8_t> compressedMap = {};
	int expectedInflatedBytes = CompressIntArray(facesWith0Water1Land, compressedMap);
	std::vector<int> inflatedMap = InflateIntArray(expectedInflatedBytes, compressedMap);

	TArray<int> response = {};
	for (int i = 0; i < inflatedMap.size(); i++) {
		response.Add(inflatedMap.size());
	}

	return response;
}
