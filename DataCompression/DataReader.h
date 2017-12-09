#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

class DataReader
{
public:
	DataReader(std::string filename);
	~DataReader();
	typedef enum ReadTypeEnum {
		Linear,
		Split3Color,
		LinearAscii,
		Unimplemented
	};
	typedef struct ReadConfigStruct {
		ReadTypeEnum type;
		uint8_t bit_count;
	};
	ReadConfigStruct getType();
	std::vector<uint16_t>& getBuffer();
	std::vector<uint16_t>& getHeader();
private:
	void readPgmFile(std::ifstream &file);
	void readTextFile(std::ifstream &file);
	std::vector<uint16_t> _buffer;
	std::vector<uint16_t> _header;
	ReadConfigStruct _config;


};

