#pragma once
#include "DataReader.h"
#include "HashDictionary.h"
#include "TableDictionary.h"
#include "DataCoder.h"

#include <memory>
using namespace std;

class LZWEngine {
public:
	int Code(const char* source, const char* dest, uint8_t indexBitCount);
	int Decode(const char* source, const char* dest);
	LZWCompressHeader& getConfig();

private:
	LZWCompressHeader _sCompressConfig;

	unique_ptr<DataReader> _pReader;
	unique_ptr<Dictionary> _pDictionary;
	unique_ptr<DataCoder> _pCoder;
};

