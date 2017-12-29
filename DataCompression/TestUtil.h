#pragma once

#include <functional>
#include "LZWEngine.h"

class TestUtil
{
public:
	TestUtil();
	~TestUtil();

	double getEntropy() const;

	double getCompressionRate() const;

	static double calculateTime(std::function<void()> fun);

	void reset();

	void runTest(const LZWEngine& engine, const char* source, const char* destination);


private:     
	double compressionRate;
	double entropy;
	double codingTime;
    double decodingTime;
};

