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



double TestUtil::getCodingEfficiency() const
{
	return codingEfficiency;
}

double TestUtil::getCompressionRate() const
{
	return compressionRatio;
}

void TestUtil::fillInputHistogram(string filename)
{
	inputHistogram.clear();
	entropy = 0.0;
	try 
	{
		reader.reset(new DataReader(filename));
	}
	catch (std::invalid_argument& exception)
	{
		std::cout << exception.what() << std::endl;
		return;
	}

	uint32_t size = 0;
	auto FileData = reader->getBuffer();
	
	for (auto& inVec : FileData)
	{
		for (auto& inputElement : inVec)
		{
			auto foundElement = std::find_if(inputHistogram.begin(), inputHistogram.end(), [inputElement](const std::pair<uint32_t, uint32_t>& pair)->bool
			{
				return inputElement == pair.first;
			});
			if (inputHistogram.end() == foundElement)
			{
				inputHistogram.push_back(std::make_pair(inputElement, 1));
			}
			else
			{
				++(foundElement->second);
			}
			++size;
		}
	}

	double probability = 0.0;

	for (auto symbol : inputHistogram)
	{
		probability = static_cast<double>(symbol.second) / static_cast<double>(size);
		entropy -= log2(probability)*probability;
	}

}

void TestUtil::fillOutputHistogram(string filename)
{
	string extension = filename.substr(filename.find_last_of("."));
	outputHistogram.clear();
	bitRate = 0.0;
	if (extension != ".lzw")
	{
		std::cout << "Not lzw file." << std::endl;
		return;
	}
	try
	{
		reader.reset(new DataReader(filename));
	}
	catch (std::invalid_argument exception)
	{
		std::cout << exception.what() << std::endl;
		return;
	}
	uint32_t size = 0;
	auto FileData = reader->getBuffer();

	for (auto& inVec : FileData)
	{
		for (auto& inputElement : inVec)
		{
			auto foundElement = std::find_if(outputHistogram.begin(), outputHistogram.end(), [inputElement](const std::pair<uint32_t, uint32_t>& pair)->bool
			{
				return inputElement == pair.first;
			});
			if (outputHistogram.end() == foundElement)
			{
				outputHistogram.push_back(std::make_pair(inputElement, 1));
			}
			else
			{
				++(foundElement->second);
			}
			++size;
		}
	}

	double probability = 0.0;
	double bitCount = reader->getConfig().indx_bit_count;

	for (auto symbol : outputHistogram)
	{
		probability = static_cast<double>(symbol.second) / static_cast<double>(size);
		bitRate += probability*bitCount;
	}

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
	catch (std::exception exception) 
		{
			std::cout << exception.what() << std::endl;
		}
	auto finish = std::chrono::steady_clock::now();
	double elapsedTime = std::chrono::duration_cast<
		std::chrono::duration<double> >(finish - start).count();
	return elapsedTime;
}

void TestUtil::reset()
{
	compressionRatio = 0.0;
	codingEfficiency = 0.0;
	entropy = 0.0;
	bitRate = 0.0;
	codingTime = 0.0;
	decodingTime = 0.0;

	inputHistogram.clear();
	outputHistogram.clear();
}

void TestUtil::runTest(LZWEngine* engine, const char* source, const char* destination, const char* result)
{
	reset();
	codingTime = calculateTime([engine, source, destination]()->double {return engine->Code(source, destination); });
	decodingTime = calculateTime([engine, destination, result]()->double {return engine->Decode(destination, result); });
	compressionRatio = static_cast<double>(getFileSizeInBytes(source)) / static_cast<double>(getFileSizeInBytes(destination));
	fillInputHistogram(source);
	fillOutputHistogram(destination);
	codingEfficiency = bitRate / entropy;
	std::cout << "Compression ratio: " << compressionRatio << std::endl;
	std::cout << "Coding time: " << codingTime << " sec." << std::endl;
	std::cout << "Decoding time: " << decodingTime << " sec." << std::endl;
	std::cout << "Entropy: " << entropy << std::endl;
	std::cout << "Bit rate: " << bitRate << std::endl;
	std::cout << "Compression efficiency: " << codingEfficiency << std::endl;

}

void TestUtil::saveToFile(string filename)
{
	ofstream testFile;

	filename = filename.substr(0,filename.find_last_of(".")) + ".txt";

	testFile.open(filename, ios::out | ios::trunc);
	testFile << "Compression ratio: " << compressionRatio << std::endl;
	testFile << "Coding time: " << codingTime << " sec." << std::endl;
	testFile << "Decoding time: " << decodingTime << " sec." << std::endl;
	testFile << "Entropy: " << entropy << std::endl;
	testFile << "Bit rate: " << bitRate << std::endl;
	testFile << "Compression efficiency: " << codingEfficiency << std::endl;
}
