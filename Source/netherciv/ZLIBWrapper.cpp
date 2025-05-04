// Fill out your copyright notice in the Description page of Project Settings.


#include "ZLIBWrapper.h"

#include "CoreMinimal.h"

#include "zlib-1.3.1/zlib.h"

ZLIBWrapper::ZLIBWrapper()
{
}

ZLIBWrapper::~ZLIBWrapper()
{
}

void add_buffer_to_vector(std::vector<uint8_t>& vector, const uint8_t* buffer, int length) {
	for (int character_index = 0; character_index < length; character_index++) {
		uint8_t current_character = buffer[character_index];
		vector.push_back(current_character);
	}
}

void ZLIBWrapper::InflateMemory(std::vector<uint8_t> source, std::vector<uint8_t>& destination, int expectedDestinationLength) {
		uLongf destination_length = compressBound(expectedDestinationLength);

		uint8_t* destination_data = (uint8_t*)malloc(destination_length);
		if (destination_data == nullptr) {
			//return Z_MEM_ERROR;
			UE_LOG(LogTemp, Display, TEXT("Some kinda memory error"));
		}

		Bytef* source_data = (Bytef*)source.data();
		int return_value = uncompress((Bytef*)destination_data, &destination_length, source_data, source.size());

		add_buffer_to_vector(destination, destination_data, destination_length);

		free(destination_data);
		//return return_value;
}

void ZLIBWrapper::CompressMemory(void* in_data, size_t in_data_size, std::vector<uint8_t>& out_data)
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

		if (res != Z_OK) {
			UE_LOG(LogTemp, Display, TEXT("FAIL ASSERT HERE"));
		}

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

	if (deflate_res != Z_STREAM_END) {
		UE_LOG(LogTemp, Display, TEXT("FAIL ASSERT 2 HERE"));
	}

	buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	out_data.swap(buffer);
}