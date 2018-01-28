#pragma once

#include <functional>
#include <vector>
#include "DataReader.h"
#include "LZWEngine.h"

class TestUtil
{
public:
	TestUtil();
	~TestUtil();

	double getCodingEfficiency() const;

	double getCompressionRate() const;

	void fillInputHistogram(string sourceFile, unsigned int rank);

	void fillOutputHistogram(string destFile);

	static unsigned long long getFileSizeInBytes(const char* file);

	static double calculateTime(std::function<void()> fun);

	void reset();

	void runTest(LZWEngine* engine, const char* source, const char* destination, const char* result, const int& dictSize);

	void saveToFile(string filename, LZWEngine* engine);

	std::string csvEntry(string filename);
	void csvHistogramEntry(string filename);

	string getCsvHeader();

	bool entropyCalculated = false;

private:     
	double compressionRatio;
	double entropy;
	double entropy2;
	double entropy3;
	double bitRate;
	double codingEfficiency;
	double codingTime;
    double decodingTime;

	int dictionarySize;

	std::vector<uint32_t> inputHistogram;
	std::vector<uint32_t> outputHistogram;

	unique_ptr<DataReader> reader; 
	LZWEngine* eng;
};

