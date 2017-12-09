#pragma once
#include <vector>
#include <string>

class DataReader
{
public:
	DataReader(std::string filename);
	~DataReader();
	typedef enum ReadTypeEnum {
		Linear,
		Split3Color,
		Unimplemented
	};
	typedef struct ReadConfigStruct {
		ReadTypeEnum type;
		uint8_t bit_count;
	};
	ReadConfigStruct getType();
	std::vector<uint16_t>& getBuffer();

};

