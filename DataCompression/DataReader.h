#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <bitset>
#include <exception>
#include "defines.h"

class DataReader
{
public:
	DataReader(std::string filename);
	~DataReader();

	LZWCompressHeader getConfig();
	uint32_t getSymbol(uint32_t bitCount);
	std::vector<uint8_t> getBuffer();
	bool isDecoding()
	{
		return _isDecoding;
	}
private:
	void readRawFile(std::ifstream &file);
	void readLzwFile(std::ifstream &file);
	void clearBuffer();
	void loadBuffer(std::ifstream &file);
	uint8_t *_buffer = NULL;
	uint32_t _fileSize = 0;
	uint32_t _shift = 0;
	uint32_t _bitOffset = 0;
	LZWCompressHeader _config;
	bool _isDecoding = false;

};

