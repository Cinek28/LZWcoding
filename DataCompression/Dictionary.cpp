#include "Dictionary.h"

#include <stdexcept>
#include <math.h>

using namespace std;

Dictionary::Dictionary(uint8_t entryBitSize, uint8_t outputBitSize, bool listOverflow)
	: _currentIndexNumber(0),
	_entryBitSize(entryBitSize),
	_outputBitSize(outputBitSize),
	_flushFlag(false),
	_listOverflow(listOverflow)
{
	if (entryBitSize > 12 || entryBitSize > outputBitSize)
	{
		throw invalid_argument("Wrong entry bit size.");
	}

	initializeValues();
}

Dictionary::~Dictionary()
{}

void Dictionary::initializeValues()
{
	_alphabetSize = pow(2, _entryBitSize);
	_maxIndexNumber = pow(2, _outputBitSize) - 1;

	_overflowFlag = _maxIndexNumber;
	_maxIndexNumber = _maxIndexNumber - FLAGS_NUMBER;
}

uint32_t Dictionary::getOverflowFlag()
{
	return _overflowFlag;
}

uint8_t Dictionary::getBitsNumber()
{
	//return static_cast<uint8_t>(ceil(log2(static_cast<double>(_currentIndexNumber))));
	//return static_cast<uint8_t>(log2(static_cast<double>(_currentIndexNumber))) +1;

	if (!_flushFlag)
	{
		return static_cast<uint8_t>(ceil(log2(static_cast<double>(_currentIndexNumber + 1))));
	}
	else
	{
		_flushFlag = false;
		return 8;
		return static_cast<uint8_t>(log2(static_cast<double>(_maxIndexNumber)));
	}
}
