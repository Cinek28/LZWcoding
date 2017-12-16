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
	std::vector<uint16_t>& getBuffer();
	std::vector<uint8_t>& getHeader();
	bool isDecoding()
	{
		return _isDecoding;
	}
private:
	void readPgmFile(std::ifstream &file);
	void readTextFile(std::ifstream &file);
	void readLzwFile(std::ifstream &file);
	std::vector<uint16_t> _buffer;
	std::vector<uint8_t> _header;
	LZWCompressHeader _config;
	bool _isDecoding = false;

};

