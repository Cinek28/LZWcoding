#include "DataCoder.h"


using std::string;
using std::ofstream;
using std::ios;
using std::vector;

DataCoder::DataCoder(string filename):
	_bitBuffer(0),
	_shift(0)
{

	_file.open(filename, ofstream::out | ofstream::binary);
	if (!_file.is_open())
		throw std::invalid_argument("Cannot open file for write");
	
}


DataCoder::~DataCoder()
{
	//Write remainder to file and close//s
	if (_shift > 0)
	{
		_file.put((char)(_bitBuffer & BYTE_MASK));
	}
	_file.close();
}

void DataCoder::writeIndex(uint32_t index, uint32_t bitSize)
{
	if (bitSize > MAX_OUTPUT_BIT_SIZE)
		throw std::out_of_range("Output bit size is out of range");
	if (bitSize == 0)
	{
		//Debug mode passthrough//
		_file << "(" << index << ")";
		return;
	}
	uint32_t indexMask = (uint32_t)(((uint64_t)1 << bitSize) - 1);
	//Check bounds of index data vs bit count//
	if ((index & indexMask) != index)
	{
		throw std::out_of_range("Index out of maximum acceptable bit range");
	}

	_bitBuffer |= (uint64_t)index << _shift;
	_shift += bitSize;
	while (_shift > BYTE_BITS)
	{
		_file.put((char)(_bitBuffer & BYTE_MASK));
		_bitBuffer = _bitBuffer >> BYTE_BITS;
		_shift -= BYTE_BITS;
	}

}
void DataCoder::writeCompressionHeader(LZWCompressHeader header)
{

	uint8_t data_size = sizeof(LZWCompressHeader);
	if (data_size > 255)
		throw std::out_of_range("Compression structure size too large");
	
	_file.put((char)data_size);
	char *charHeader = reinterpret_cast<char*>(&header);
	for (int i = 0; i < data_size; ++i)
	{
		_file.put(charHeader[i]);
	}
}
//
//void DataCoder::writeUncompressedData(vector<uint8_t> data)
//{
//	if (data.size() == 0)
//	{
//		std::cout << "No data specified to write as uncompressed";
//		return; 
//	}
//	char *buf;
//	buf = new char[data.size()];
//	for (int i = 0; i < data.size(); ++i)
//	{
//		buf[i] = data[i];
//	}
//	_file.write(buf, data.size());
//	delete[] buf;
//}