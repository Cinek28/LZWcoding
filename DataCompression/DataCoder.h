#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include "defines.h"
class DataCoder
{
public:
	DataCoder(std::string filename, uint8_t outputBitSize);
	~DataCoder();
	void writeIndex(uint32_t index);
	void writeCompressionHeader(/*Placeholder for some structure maybe*/);
	void writeUncompressedData(std::vector<uint8_t> header);
private:
	std::ofstream _file;
	const uint8_t _byteSize = 8;
	uint8_t _outputBitSize;
	uint32_t _indexMask;
	uint32_t _shiftRemainder;
	uint8_t _dataRemainder;
};

