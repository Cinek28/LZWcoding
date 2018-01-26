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
	if (extension == ".lzw")
	{
		readLzwFile(file);
	}
	else
	{
		readRawFile(file);
	}
	file.close();
}
void DataReader::loadBuffer(std::ifstream &file)
{
	if (_buffer != NULL)
	{
		clearBuffer();
	}
	std::streampos begin, end;
	file.seekg(0, ios::beg);
	begin = file.tellg();
	file.seekg(0, ios::end);
	end = file.tellg();
	file.seekg(0, ios::beg);
	_fileSize = end - begin;
	if (_fileSize == 0)
		throw std::out_of_range("File size equals 0, error");
	_buffer = new uint8_t[_fileSize];
	file.read((char*)_buffer, _fileSize);
}
void DataReader::clearBuffer()
{
	delete[] _buffer;
	_fileSize = 0;
	_buffer = NULL;
	_bitOffset = 0;
}
DataReader::~DataReader()
{
	clearBuffer();
}
void DataReader::readRawFile(std::ifstream &file)
{
	loadBuffer(file);
}

void DataReader::readLzwFile(std::ifstream &file)
{
	_isDecoding = true;
	loadBuffer(file);
	//Read up the input structure//
	//first byte is the input structure byte size//
	uint8_t comp_header_size = _buffer[0];
	//Read compression header//
	if (comp_header_size != sizeof(LZWCompressHeader))
		throw std::invalid_argument("Compression header incompatible");
	LZWCompressHeader compHeader;
	char *charHeader = reinterpret_cast<char*>(&compHeader);
	for (int i = 1; i < comp_header_size + 1; ++i)
	{
		 charHeader[i-1] = _buffer[i];
	}
	_config = compHeader;
	_bitOffset = 8 * (comp_header_size + 1);
	
}

//flips from big to little or vice versa
int flip(int x) {
	char temp, *t = (char*)&x;
	temp = t[0];
	t[0] = t[3];
	t[3] = temp;
	temp = t[1];
	t[1] = t[2];
	t[2] = temp;
	return x;
}
uint32_t DataReader::getSymbol(uint32_t bitCount)
{
	uint8_t *data = _buffer;
	uint32_t bitOffset = _bitOffset;
	_bitOffset += bitCount;
	if (_bitOffset > 8 * _fileSize)
		throw std::out_of_range("Cannot get more bits, EOF");
	if (bitCount >= MAX_OUTPUT_BIT_SIZE)
		throw std::invalid_argument("DataReader::bit count size too large!");
	bitCount = (uint32_t)(((uint64_t)1 << bitCount) - 1); 
	data += bitOffset / 8;
	bitOffset %= 8;
	return (*((uint32_t*)data) >> bitOffset) & bitCount;  //little endian
	//return (flip(data[0]) >> bitOffset) & numBits; //big endian
}
LZWCompressHeader DataReader::getConfig()
{
	return _config;
}
vector<uint8_t> DataReader::getBuffer()
{
	vector<uint8_t> dataVector;
	for (int i = 0; i < _fileSize; ++i)
		dataVector.push_back(_buffer[i]);
	return dataVector;
}