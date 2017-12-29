#pragma once

#include <functional>
#include "LZWEngine.h"

class TestUtil
{
public:
	TestUtil();
	~TestUtil();

	double getCodingEfficiency() const;

	double getCompressionRate() const;

	static unsigned long long getFileSizeInBytes(const char* file);

	static double calculateTime(std::function<void()> fun);

	void reset();

	void runTest(LZWEngine* engine, const char* source, const char* destination, const char* result);


private:     
	double compressionRatio;
	double codingEfficiency;
	double codingTime;
    double decodingTime;
};

