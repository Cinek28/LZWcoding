#include "Dictionary.h"

#include <stdexcept>
#include <math.h>

using namespace std;

Dictionary::Dictionary(uint8_t entryBitSize, uint8_t outputBitSize)
	: _currentIndexNumber(0),
	_entryBitSize(entryBitSize),
	_outputBitSize(outputBitSize)
{
	if (entryBitSize > 12 || entryBitSize > outputBitSize)
	{
		throw invalid_argument("Wrong entry bit size.");
	}

	initializeValues();
	initializeAlphabet();
}

Dictionary::~Dictionary()
{
}

void Dictionary::initializeValues()
{
	_alphabetSize = pow(2, _entryBitSize);
	_maxIndexNumber = pow(2, _outputBitSize);

	overflowFlag = _maxIndexNumber;
	_maxIndexNumber = _maxIndexNumber - FLAGS_NUMBER;
}

void Dictionary::initializeAlphabet()
{
	for (_currentIndexNumber; _currentIndexNumber < _alphabetSize; ++_currentIndexNumber)
	{
		_container[vector<uint16_t>(static_cast<uint16_t>(_currentIndexNumber))] = _currentIndexNumber;
	}
}

void Dictionary::flush()
{
	_container.clear();
}

bool Dictionary::getEntry(vector<uint16_t> word, uint32_t& index)
{
	bool state;

	auto containerElement = _container.find(word);

	if (containerElement == _container.end() || containerElement->second != overflowFlag)
	{
		state = false;
	}
	else
	{
		state = true;
		index = containerElement->second;
	}

	return state;
}

uint32_t Dictionary::insertEntry(vector<uint16_t> word)
{
	uint32_t index;

	if (_container.size() > _maxIndexNumber)
	{
		index = overflowFlag;
		flush();
	}
	else
	{
		index = ++_currentIndexNumber;
		_container[word] = index;
	}

	return index;
}

uint32_t Dictionary::getOverflowFlag()
{
	return overflowFlag;
}
