#include "HashDictionary.h"
#include <algorithm>

using namespace std;

HashDictionary::HashDictionary(uint8_t entryBitSize, uint8_t outputBitSize)
	: Dictionary(entryBitSize, outputBitSize)
{
	initializeAlphabet();
}


HashDictionary::~HashDictionary()
{
}

void HashDictionary::initializeAlphabet()
{
	for (_currentIndexNumber; _currentIndexNumber < _alphabetSize; ++_currentIndexNumber)
	{
		_container[vector<uint16_t>{static_cast<uint16_t>(_currentIndexNumber)}] = _currentIndexNumber;
	}
}

bool HashDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index)
{
	bool state;
	auto containerElement = _container.find(word);

	if (containerElement == _container.end() || containerElement->second > _maxIndexNumber)
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

uint32_t HashDictionary::insertEntry(std::vector<uint16_t>& word)
{
	if (_currentIndexNumber > _maxIndexNumber)
	{
		flush();
		return _overflowFlag;
	}
	else
	{
		_container[word] = _currentIndexNumber;
		return _currentIndexNumber++;
	}
}

bool HashDictionary::getEntry(uint32_t index, std::vector<uint16_t>& word)
{
	for each (auto pair in _container)
	{
		if (pair.second == index)
		{
			word = pair.first;
			return true;
		}
	}
	return false;
}

void HashDictionary::flush()
{
	_container.clear();
	_currentIndexNumber = 0;
	initializeAlphabet();
}


