#include "TestUtil.h"
#include <chrono>
#include <functional>


TestUtil::TestUtil()
{
}

TestUtil::~TestUtil()
{
}



double TestUtil::getEntropy()
{
	return 0.0f;
}

double TestUtil::getCompressionRate()
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
	codingTime = 0;
	decodingTime = 0;

}
