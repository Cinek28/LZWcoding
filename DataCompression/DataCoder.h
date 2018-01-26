#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include "defines.h"
#include <bitset>
class DataCoder
{
public:
	DataCoder(std::string filename);
	~DataCoder();
	void writeIndex(uint32_t index, uint32_t bitSize);
	void writeCompressionHeader(LZWCompressHeader header);
private:
	std::ofstream _file;
	const uint8_t _byteSize = 8;
	uint64_t _bitBuffer = 0;
	uint32_t _shift = 0;
};

