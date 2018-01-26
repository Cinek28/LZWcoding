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
	
		for (int i = 0; i < FileData.size(); i = i + rank)
		{
			auto foundElement = std::find_if(inputHistogram.begin(), inputHistogram.end(), [&](const std::pair<uint32_t*, uint32_t>& pair)->bool
			{
				for (int j = 0; j < rank; ++j) 
				{
					if (*(pair.first + j*sizeof(uint8_t)) != FileData[i + j])
						return false;
				}
				return true;
			});
			if (inputHistogram.end() == foundElement)
			{
				inputHistogram.push_back(std::make_pair(&FileData[i], 1));
			}
			else
			{
				++(foundElement->second);
			}
			++size;
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

		for (auto& inputElement : FileData)
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
		}

	double probability = 0.0;
	unsigned int size = _pBitCountVector->size();
	for (int i = 0; i< _pBitCountVector->size(); ++i)
	{
		auto symbol = std::find_if(outputHistogram.begin(), outputHistogram.end(), [&](auto item)->bool {
			return item.first == FileData[i];
		});
		probability = static_cast<double>(symbol.second) / static_cast<double>(size);
		bitRate += probability*(*_pBitCountVector)[i];
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

void TestUtil::runTest(LZWEngine* engine, const char* source, const char* destination, const char* result, uint8_t indexBitCount)
{
	reset();
	codingTime = calculateTime([engine, source, destination, indexBitCount]()->double {return engine->Code(source, destination, indexBitCount); });
	decodingTime = calculateTime([engine, destination, result]()->double {return engine->Decode(destination, result); });
	compressionRatio = static_cast<double>(getFileSizeInBytes(source)) / static_cast<double>(getFileSizeInBytes(destination));
	fillInputHistogram(source);
	fillOutputHistogram(destination);
	codingEfficiency = bitRate / entropy;
	std::cout << "Plik: " << source << std::endl;
	std::cout << "Ustalona liczba bitów na indeks s³ownika: " << engine->getConfig().indx_bit_count <<" bit." << std::endl;
	std::cout << "Stopieñ kompresji: " << compressionRatio << std::endl;
	std::cout << "Czas kodowania: " << codingTime << " sek." << std::endl;
	std::cout << "Czas dekodowania: " << decodingTime << " sek." << std::endl;
	std::cout << "Entropia: " << entropy << " bit" << std::endl;
	std::cout << "Œrednia d³ugoœæ bitowa: " << bitRate << "bit" << std::endl;
	std::cout << "Efektywnoœæ kodowania: " << codingEfficiency << std::endl;

}

void TestUtil::saveToFile(string filename, LZWEngine* engine)
{
	ofstream testFile;

	string file = filename.substr(0,filename.find_last_of(".")) + ".txt";

	testFile.open(file, ios::out | std::ios::binary);
	testFile << "Stopien kompresji: " << compressionRatio << std::endl << std::endl;
	testFile << "Czas kodowania: " << codingTime << " sek." << std::endl << std::endl;
	testFile << "Czas dekodowania: " << decodingTime << " sek." << std::endl << std::endl;
	testFile << "Entropia: " << entropy << " bit" << std::endl << std::endl;
	testFile << "Srednia dlugosc bitowa: " << bitRate << "bit" << std::endl << std::endl;
	testFile << "Efektywnosc kodowania: " << codingEfficiency << std::endl << std::endl;
	testFile.close();
}
string TestUtil::csvEntry(string filename)
{
	std::stringstream ss;
	ss << filename << "," << compressionRatio << "," << codingTime << "," << decodingTime << "," << entropy << "," << bitRate << "," << codingEfficiency << "\n";
	return ss.str();
}
string TestUtil::getCsvHeader()
{
	return "\"sep=,\"\nfilename,compressionRatio,compressionTime[s],decompressionTime[s],entropy[bit],bitRate[bit],efficiency\n";
}
