#include "TestUtil.h"
#include <chrono>
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

TestUtil::TestUtil()
{
}

TestUtil::~TestUtil()
{
}



double TestUtil::getEntropy() const
{
	return entropy;
}

double TestUtil::getCompressionRate() const
{
	return compressionRate;
}

unsigned long long TestUtil::getFileSizeInBytes(const char* file)
{
	filesystem::path path(file);
	path = filesystem::canonical(path);
	std::cout << "File " << path.u8string() << " size: " << filesystem::file_size(path) << " bytes" <<  std::endl;
	return filesystem::file_size(path);
}

double TestUtil::calculateTime(std::function<void()> fun)
{
	auto start = std::chrono::steady_clock::now();
	try {
			fun();
		}
	catch (std::exception e) 
		{
			std::cout << e.what() << std::endl;
		}
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

void TestUtil::runTest(LZWEngine* engine, const char* source, const char* destination, const char* result)
{
	this->reset();
	codingTime = calculateTime([engine, source, destination]()->double {return engine->Code(source, destination); });
	decodingTime = calculateTime([engine, destination, result]()->double {return engine->Decode(destination, result); });
	compressionRate = static_cast<double>(getFileSizeInBytes(source)) / static_cast<double>(getFileSizeInBytes(destination));
	std::cout << "Compression rate: " << compressionRate << std::endl;
	std::cout << "Coding time: " << codingTime << " sec." << std::endl;
	std::cout << "Decoding time: " << decodingTime << " sec." << std::endl;
	std::cout << "Entropy: " << entropy << std::endl;

}
