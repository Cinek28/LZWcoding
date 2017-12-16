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
		_container[basic_string<char16_t>{static_cast<uint16_t>(_currentIndexNumber)}] = _currentIndexNumber;
	}
}

bool HashDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index)
{
	bool state;
	basic_string<char16_t> stringWord(word.begin(), word.end());

	auto containerElement = _container.find(stringWord);
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
	basic_string<char16_t> stringWord(word.begin(), word.end());
	if (_currentIndexNumber > _maxIndexNumber)
	{
		flush();
		return _overflowFlag;
	}
	else
	{
		_container[stringWord] = _currentIndexNumber;
		return _currentIndexNumber++;
	}
}

bool HashDictionary::getEntry(uint32_t index, std::vector<uint16_t>& word)
{
	for each (auto pair in _container)
	{
		if (pair.second == index)
		{
			word = vector<uint16_t>{ pair.first.begin(), pair.first.end() };
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


