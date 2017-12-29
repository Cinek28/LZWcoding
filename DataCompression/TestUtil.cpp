#include "TestUtil.h"
#include <chrono>

TestUtil::TestUtil()
{
}

TestUtil::~TestUtil()
{
}



double TestUtil::getEntropy() const
{
	return 0.0f;
}

double TestUtil::getCompressionRate() const
{
	return 0.0f;
}

double TestUtil::calculateTime(std::function<void()> fun)
{
	auto start = std::chrono::steady_clock::now();
	fun();
	auto finish = std::chrono::steady_clock::now();
	double elapsedTime = std::chrono::duration_cast<
		std::chrono::duration<double> >(finish - start).count();
	return elapsedTime;
}

void TestUtil::reset()
{
	compressionRate = 0.0;
	entropy = 0.0;
	codingTime = 0.0;
	decodingTime = 0.0;

}

void TestUtil::runTest(const LZWEngine& engine, const char* source, const char* destination)
{
	this->reset();
	codingTime = calculateTime([](const LZWEngine&, )->double {return });
	decodingTime = calculateTime();
	std::cout << codingTime;

}
