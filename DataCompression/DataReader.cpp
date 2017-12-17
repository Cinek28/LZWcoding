#include "DataReader.h"


using std::string;
using std::vector;
using std::ios;
using std::ifstream;
using std::cout;

DataReader::DataReader(string filename)
{
	string extension = filename.substr(filename.find_last_of("."));
	cout << "Filename extension detected: " << extension << std::endl;
	ifstream file(filename, ios::in | ios::binary);
	if (!file.is_open())
		throw std::invalid_argument("File does not exist");
	clearBuffer();
	if (extension == ".txt")
	{
		readTextFile(file);
	}
	else if (extension == ".pgm")
	{
		readPgmFile(file);
	}
	else if (extension == ".lzw")
	{
		readLzwFile(file);
	}
	else
	{
		_config.word_bit_count = 8;
		_config.data_order_type = ReadTypeEnum::Linear;
	}
	file.close();
}

void DataReader::clearBuffer()
{
	for (int i = 0; i < _buffer.size(); ++i)
		_buffer[i].clear();
	_buffer.clear();
}
DataReader::~DataReader()
{
	clearBuffer();
}
void DataReader::readTextFile(std::ifstream &file)
{
	_config.word_bit_count = 8;
	_config.data_order_type = ReadTypeEnum::LinearAscii;
	_buffer.push_back(vector<uint32_t>());
	char c;
	while (file.get(c))
	{
		uint8_t byte = (uint8_t)c;
		_buffer[0].push_back((uint32_t)byte);
	}
}

void DataReader::readPgmFile(std::ifstream &file)
{
	_config.word_bit_count = 8;
	_config.data_order_type = ReadTypeEnum::Linear;
	_buffer.push_back(vector<uint32_t>());
	//read and copy header info of pgm file//
	int PGM_HEADER_LINES = 3;
	int pgm_maxval = 255;
	string pgm_type;
	int content_line = 0;
	while (content_line < PGM_HEADER_LINES)
	{
		string line;
		std::getline(file, line);
		std::cout << line << std::endl;
		if (content_line == 2)
		{
			std::istringstream ss(line);
			ss >> pgm_maxval;
		}
		for (int j = 0; j < line.size(); ++j)
		{
			_buffer[0].push_back(line[j]);
		}
		_buffer[0].push_back('\n');
		if (line[0] == '#')
		{
			//Handle comment lines//
		}
		else
		{
			if (content_line == 0)
				pgm_type = line;
			content_line++;
		}
	}
	if (pgm_type == "P6")
	{
		//_config.data_order_type = ReadTypeEnum::Split3Color; - currently niot implemented in coder (for decompression)
		_config.data_order_type = ReadTypeEnum::Linear;
	}
	//Get values to the buffer and reassemble as correct bitness//
	_buffer.push_back(vector<uint32_t>());
	char c;
	if (_config.data_order_type = ReadTypeEnum::Linear)
		while (file.get(c))
		{
			uint8_t byte = (uint8_t)c;
			_buffer[1].push_back((uint32_t)byte);
		}
	else if (_config.data_order_type = ReadTypeEnum::Split3Color)
	{
		vector<uint8_t> colors[3];
		int color = 0;
		while (file.get(c))
		{
			uint8_t byte = (uint8_t)c;
			colors[color++].push_back(byte);
			if (color >= 3)
				color = 0;
		}
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < colors[i].size(); ++j)
			{
				_buffer[1].push_back((uint32_t)colors[i][j]);
			}
	}
}

void DataReader::readLzwFile(std::ifstream &file)
{
	_isDecoding = true;
	//Read up the input structure//
	//first byte is the input structure byte size//
	uint8_t comp_header_size = 0;
	if (!file.get((char&)comp_header_size))
		throw std::invalid_argument("File empty");
	//Read compression header//
	if (comp_header_size != sizeof(LZWCompressHeader))
		throw std::invalid_argument("Compression header incompatible");
	LZWCompressHeader compHeader;
	uint8_t byte;
	char *charHeader = reinterpret_cast<char*>(&compHeader);
	for (int i = 0; i < comp_header_size; ++i)
	{
		 if(!file.get((char&)byte))
			 throw std::invalid_argument("File finished prematurely");
		 charHeader[i] = byte;
	}
	_config = compHeader;
	_buffer.push_back(vector<uint32_t>());
	//Decoding stage//
	uint32_t lowerMask, higherMask;
	uint32_t remainder = 0, shift = 0;
	const uint8_t byteSize = 8;
	uint32_t indexMask = 0;
	uint32_t outputIndex = 0;
	int bitsToProcess;
	//Reassemble 8 bit to output bit size//
	for (int i = 0; i < _config.indx_bit_count; ++i)
	{
		indexMask |= 1 << i;
	}
	while (file.get((char&)byte))
	{

		//std::bitset<8> x(byte);
		//std::cout<<"R: " << x << std::endl;
		bitsToProcess = byteSize;
		while (bitsToProcess > 0)
		{
			//Gather remainder bits from previous stage//
			higherMask = (indexMask >> shift) << shift;
			lowerMask = indexMask ^ higherMask;
			uint32_t noBitsToWrite = byteSize + shift;
			uint32_t newDataBits = _config.indx_bit_count - shift;
			if (noBitsToWrite >= _config.indx_bit_count)
			{
				outputIndex = lowerMask & remainder;
				outputIndex |= ((higherMask >> shift) & byte) << shift;
				_buffer[0].push_back((uint32_t)outputIndex);
				remainder = (byte & 0xFF) >> newDataBits;
				shift = byteSize - newDataBits;
				bitsToProcess -= _config.indx_bit_count;
				//std::bitset<14> x(outputIndex);
				//std::cout << "A: " << x << " - " << outputIndex << std::endl;
				//std::bitset<14> y(remainder);
				//std::cout << "S: ";
				//for (int i = shift-1; i >= 0; --i)
				//	std::cout << y[i];
				//std::cout << std::endl;
			}
			else
			{
				remainder |= byte << shift;
				shift += byteSize;
				bitsToProcess -= 8;

				//std::bitset<14> x(remainder );
				//std::cout << "S: ";
				//for (int i = shift-1; i >= 0; --i)
				//	std::cout << x[i];
				//std::cout <<std::endl;
			}
		}
	}
}
LZWCompressHeader DataReader::getConfig()
{
	return _config;
}
vector<std::vector<uint32_t>>& DataReader::getBuffer()
{
	return _buffer;
}