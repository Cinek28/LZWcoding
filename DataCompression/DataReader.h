#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "defines.h"

class DataReader
{
public:
	DataReader(std::string filename);
	~DataReader();

	LZWCompressHeader getConfig();
	std::vector<std::vector<uint16_t>>& getBuffer();
	bool isDecoding()
	{
		return _isDecoding;
	}
private:
	void readPgmFile(std::ifstream &file);
	void readTextFile(std::ifstream &file);
	void readLzwFile(std::ifstream &file);
	void clearBuffer();
	std::vector<std::vector<uint16_t>> _buffer;
	LZWCompressHeader _config;
	bool _isDecoding = false;

};

