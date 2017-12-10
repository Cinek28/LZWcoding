#include "DataCoder.h"


using std::string;
using std::ofstream;
using std::ios;
using std::vector;

DataCoder::DataCoder(string filename, uint8_t outputBitSize):
	_outputBitSize(outputBitSize)
{
	if (outputBitSize > MAX_OUTPUT_BIT_SIZE)
		throw std::out_of_range("Output bit size is out of range");
	else if (outputBitSize == 0)
		std::cout << "Debug mode entered, index passthrough" << std::endl;

	_file.open(filename, ofstream::out | ofstream::binary);
	if (!_file.is_open())
		throw std::invalid_argument("Cannot open file for write");
	
	//Create a bit mask of acceptable values//
	_indexMask = 0;
	for (int i = 0; i < _outputBitSize; ++i)
	{
		_indexMask |= 1 << i;
	}
}


DataCoder::~DataCoder()
{
	//Write remainder to file and close//
	if (_shiftRemainder > 0)
	{
		//SanityCheck//
		if (_shiftRemainder > _byteSize)
			throw std::exception("Bit shift logic invalid");
		uint8_t data = (uint8_t)_dataRemainder;
		_file.put((char)data);
	}
	_file.close();
}

void DataCoder::writeIndex(uint32_t index)
{
	if (_outputBitSize == 0)
	{
		//Debug mode passthrough//
		_file << "(" << index << ")";
		return;
	}
	//Check bounds of index data vs bit count//
	else if ((index & _indexMask) != index)
	{
		throw std::out_of_range("Index out of maximum acceptable bit range");
	}

	uint32_t lowerMask, higherMask, shift;
	uint8_t dataByte;
	int bitsToWrite = (_outputBitSize + _shiftRemainder);
	int dataBitsWritten = 0;
	while (bitsToWrite > 0)
	{
		shift = _shiftRemainder;
		higherMask = (_indexMask >> _shiftRemainder) << _shiftRemainder; //Clear lower bits
		lowerMask = _indexMask ^ higherMask;

		
		//SanityCheck//
		if (_shiftRemainder > _byteSize)
			throw std::exception("Bit shift logic invalid");

		//Merge old data with new data//
		uint32_t newDataBits = _byteSize - _shiftRemainder;
		dataByte = 0;
		dataByte = lowerMask & _dataRemainder;
		dataByte |= ((higherMask >> _shiftRemainder) & (index >> dataBitsWritten)) << _shiftRemainder;
		dataBitsWritten += newDataBits;
		bitsToWrite -= _byteSize;
		_file.put(dataByte);
		if (bitsToWrite < _byteSize)
		{
			//Save the ramainder data//
			_shiftRemainder = bitsToWrite;
			_dataRemainder = index >> dataBitsWritten;
			bitsToWrite = 0;
		}
		else
		{
			_shiftRemainder = 0;
			_dataRemainder = 0;
		}
	}

}
void DataCoder::writeCompressionHeader()
{

}
void DataCoder::writeUncompressedData(vector<uint8_t> data)
{
	if (data.size() == 0)
	{
		std::cout << "No data specified to write as uncompressed";
		return; 
	}
	char *buf;
	buf = new char[data.size()];
	for (int i = 0; i < data.size(); ++i)
	{
		buf[i] = data[i];
	}
	_file.write(buf, data.size());
	delete[] buf;
}