#include "Dictionary.h"

#include <stdexcept>
#include <math.h>

using namespace std;

Dictionary::Dictionary(uint8_t entryBitSize, uint8_t outputBitSize)
	: _currentIndexNumber(0)
{
	if (entryBitSize > 12 || entryBitSize > outputBitSize)
	{
		throw invalid_argument("Wrong entry bit size.");
	}

	_alphabetSize = pow(2, entryBitSize);
	_maxIndexNumber = pow(2, outputBitSize);

	initializeAlphabet();
}

Dictionary::~Dictionary()
{
}

bool Dictionary::getEntry(vector<uint16_t> word, uint32_t& index)
{
	bool state;

	auto foundIndex = _container.find(word);

	if (foundIndex == _container.end())
	{
		state = false;
	}
	else
	{
		state = true;
		index = foundIndex->second;
	}

	return state;
}

uint32_t Dictionary::insertEntry(vector<uint16_t> word)
{
	uint32_t index;

	if (_container.size() > _maxIndexNumber)
	{
		// TO DO: return flag index
		// TO DO: dictionary overflow
	}
	else
	{
		index = ++_currentIndexNumber;
		_container[word] = index;
	}

	return index;
}

void Dictionary::initializeAlphabet()
{
	for (_currentIndexNumber; _currentIndexNumber < _alphabetSize; ++_currentIndexNumber)
	{
		_container[vector<uint16_t>(_currentIndexNumber)] = _currentIndexNumber;
	}
}
