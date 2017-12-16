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
	ifstream file(filename, ios::in | ios::binary | ios::beg);
	if (!file.is_open())
		throw std::invalid_argument("File does not exist");

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
		_readLzwFile(file);
	}
	else
	{
		_config.bit_count = 8;
		_config.type = ReadTypeEnum::Linear;
	}
	file.close();
}


DataReader::~DataReader()
{
}
void DataReader::readTextFile(std::ifstream &file)
{
	_config.bit_count = 8;
	_config.type = ReadTypeEnum::LinearAscii;
	char c;
	while (file.get(c))
	{
		uint8_t byte = (uint8_t)c;
		_buffer.push_back((uint16_t)byte);
	}
}

void DataReader::readPgmFile(std::ifstream &file)
{
	_config.bit_count = 8;
	_config.type = ReadTypeEnum::Linear;

	//read and copy header info of pgm file//
	int PGM_HEADER_LINES = 3;
	int pgm_maxval = 255;
	string pgm_type;
	int content_line = 0;
	for (int i = 0; i < PGM_HEADER_LINES; ++i)
	{
		string line;
		std::getline(file, line);

		if (content_line == 2)
		{
			std::istringstream ss(line);
			ss >> pgm_maxval;
		}
		for (int j = 0; j < line.size(); ++j)
		{
			_header.push_back(line[j]);
		}
		_header.push_back('\n');
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
		_config.type = ReadTypeEnum::Split3Color;
	}
	//Get values to the buffer and reassemble as correct bitness//
	char c;
	if (_config.type = ReadTypeEnum::Linear)
		while (file.get(c))
		{
			uint8_t byte = (uint8_t)c;
			_buffer.push_back((uint16_t)byte);
		}
	else if (_config.type = ReadTypeEnum::Split3Color)
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
				_buffer.push_back((uint16_t)colors[i][j]);
			}
	}
}

void DataReader::readLzwFile(std::ifstream &file)
{
	/*
	//Read up the input structure//
	//first byte is the input structure byte size//
	uint8_t comp_header_size = 0;
	if (!file.get((char&)comp_header_size))
		throw std::invalid_argument("File empty");
	//Read compression header//
	if (comp_header_size != sizeof(heder))
		throw std::invalid_argument("Compression header incompatible");
	heder compHeader;
	char *charHeader = reinterpret_cast<
	for (int i = 0; i < comp_header_size; ++i)
	{
		 
	}*/

}
DataReader::ReadConfigStruct DataReader::getType()
{
	return _config;
}
vector<uint16_t>& DataReader::getBuffer()
{
	return _buffer;
}
vector<uint8_t>& DataReader::getHeader()
{
	return _header;
}