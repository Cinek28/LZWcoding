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

void TestUtil::fillInputHistogram(string filename, unsigned int rank)
{
	inputHistogram = std::vector<uint32_t>(MAX_HISTOGRAM_SIZE, 0);
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

	auto FileData = reader->getBuffer();
	uint32_t size = FileData.size();
	uint32_t index = 0;
		for (int i = 0; i < size - rank + 1; i = i + rank)
		{
			switch (rank)
			{
			case 1:
				index = FileData[i];
				break;

			case 2:
				index = static_cast<uint32_t>(FileData[i]) + (static_cast<uint32_t>(FileData[i+1])<<8);
				break;
			case 3:
				index = static_cast<uint32_t>(FileData[i]) + (static_cast<uint32_t>(FileData[i + 1]) << 8) + (static_cast<uint32_t>(FileData[i + 2]) << 16);
				break;
			default:
				index = FileData[i];
			}
			++inputHistogram[index];
			index = 0;
		}

	double probability = 0.0;

	for (auto count : inputHistogram)
	{
		if (count != 0){
			probability = static_cast<double>(count) / (static_cast<double>(size)/rank);
			entropy -= log2(probability)*probability;
		}
	}

}

void TestUtil::fillOutputHistogram(string filename)
{
	bitRate = 0.0;

	size_t size = eng->_symbolBitsNumber.size();
	for (int i = 0; i < size; ++i)
	{
		bitRate += static_cast<double>(eng->_symbolBitsNumber[i].second)/size;
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
	auto start = std::chrono::high_resolution_clock::now();
	try {
			fun();
		}
	catch (std::exception exception) 
		{
			std::cout << exception.what() << std::endl;
		}
	auto finish = std::chrono::high_resolution_clock::now();
	double elapsedTime = std::chrono::duration_cast<
		std::chrono::milliseconds >(finish - start).count();
	return elapsedTime;
}

void TestUtil::reset()
{
	compressionRatio = 0.0;
	codingEfficiency = 0.0;
	entropy = 0.0;
	entropy2 = 0.0;
	entropy3 = 0.0;
	bitRate = 0.0;
	codingTime = 0.0;
	decodingTime = 0.0;

	dictionarySize = 0;

	inputHistogram.clear();
	outputHistogram.clear();
	eng = nullptr;
}

void TestUtil::runTest(LZWEngine* engine, const char* source, const char* destination, const char* result, const int& dictSize)
{
	eng = engine;
	dictionarySize = dictSize;
	codingTime = calculateTime([&]()->double {return engine->Code(source, destination, dictionarySize); });
	decodingTime = calculateTime([engine, destination, result]()->double {return engine->Decode(destination, result); });
	compressionRatio = static_cast<double>(getFileSizeInBytes(source)) / static_cast<double>(getFileSizeInBytes(destination));
	std::cout << "Plik: " << source << std::endl;
	std::cout << "Rozmiar s³ownika: " << dictionarySize << std::endl;
	std::cout << "Stopieñ kompresji: " << compressionRatio << std::endl;
	std::cout << "Czas kodowania: " << codingTime << " ms." << std::endl;
	std::cout << "Czas dekodowania: " << decodingTime << " ms." << std::endl;
	if (!entropyCalculated)
	{
		fillInputHistogram(source, 2);
		entropy2 = entropy;
		fillInputHistogram(source, 3);
		entropy3 = entropy;
		fillInputHistogram(source, 1);
		entropyCalculated = true;
	}
	fillOutputHistogram(destination);
	codingEfficiency = bitRate / entropy;
	std::cout << "Entropia: " << entropy << " bit" << std::endl;
	std::cout << "Entropia 2 rzêdu: " << entropy2 << " bit" << std::endl;
	std::cout << "Entropia 3 rzêdu: " << entropy3 << " bit" << std::endl;
	std::cout << "Œrednia d³ugoœæ bitowa: " << bitRate << "bit" << std::endl;
	std::cout << "Efektywnoœæ kodowania: " << codingEfficiency << std::endl;

}

void TestUtil::saveToFile(string filename, LZWEngine* engine)
{
	ofstream testFile;

	string file = filename.substr(0,filename.find_last_of(".")) + ".txt";

	testFile.open(file, ios::out | std::ios::binary);
	testFile << "Rozmiar slownika: " << dictionarySize << std::endl << std::endl;
	testFile << "Stopien kompresji: " << compressionRatio << std::endl << std::endl;
	testFile << "Czas kodowania: " << codingTime << " ms." << std::endl << std::endl;
	testFile << "Czas dekodowania: " << decodingTime << " ms." << std::endl << std::endl;
	testFile << "Entropia: " << entropy << " bit" << std::endl << std::endl;
	testFile << "Entropia 2 rzêdu: " << entropy2 << " bit" << std::endl << std::endl;
	testFile << "Entropia 3 rzêdu: " << entropy3 << " bit" << std::endl << std::endl;
	testFile << "Srednia dlugosc bitowa: " << bitRate << "bit" << std::endl << std::endl;
	testFile << "Efektywnosc kodowania: " << codingEfficiency << std::endl << std::endl;
	testFile.close();
}
string TestUtil::csvEntry(string filename)
{
	std::stringstream ss;
	ss << filename << "," << dictionarySize << "," << compressionRatio << "," << codingTime << "," << decodingTime << "," << entropy << "," << entropy2 << "," << entropy3 << "," << bitRate << "," << codingEfficiency << "\n";
	return ss.str();
}

void TestUtil::csvHistogramEntry(string filename) 
{
	ofstream histogramFile;
	histogramFile.open(filename);
	histogramFile << filename << "\n";
	histogramFile << "Value,Count" << "\n";
	int i = 0;
	for (int i = 0; i < 256; ++i)
	{
		histogramFile << i << "," << inputHistogram[i] << "\n";
	}
	histogramFile.close();
}
string TestUtil::getCsvHeader()
{
	return "\"sep=,\"\nfilename,dictionarySize,compressionRatio,compressionTime[ms],decompressionTime[ms],entropy[bit],entropy2[bit],entropy3[bit],bitRate[bit],efficiency\n";
}
