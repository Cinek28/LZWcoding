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

bool HashDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index, uint8_t& bitsNumber)
{
	bool state;
	basic_string<char16_t> stringWord(word.begin(), word.end());
	bitsNumber = static_cast<uint8_t>(log2(static_cast<double>(_currentIndexNumber))) + 1;

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
		return removeElementFromList(stringWord);
	}
	else
	{
		_container[stringWord] = _currentIndexNumber;
		insertElementToList(_currentIndexNumber);

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

uint32_t HashDictionary::removeElementFromList(basic_string<char16_t> stringWord)
{
	auto iter = _indexList.begin();
	uint32_t removedIndex = (*iter).second;

	// Remove from list.
	_indexList.erase(iter);

	// Remove from dictionary.
	bool removedFlag = true;
	for (auto containerIter = _container.begin(); containerIter != _container.end() && removedFlag;)
	{	
		auto prevIter = containerIter;
		++containerIter;
		if ((*containerIter).second == removedIndex)
		{
			_container.erase(prevIter);
			removedFlag = false;
		}
	}

	// Add new element to dictionary.
	_container[stringWord] = removedIndex;

	return removedIndex;
}

void HashDictionary::insertElementToList(uint32_t currenIndex)
{
	_indexList.push_back(make_pair(1, currenIndex));
}


