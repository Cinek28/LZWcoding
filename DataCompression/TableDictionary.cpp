#include "TableDictionary.h"
#include <math.h>

using namespace std;

TableDictionary::TableDictionary(uint8_t entryBitSize, uint8_t outputBitSize, bool listOverflow)
	: Dictionary(entryBitSize, outputBitSize, listOverflow)
{
	initializeAlphabet();
}

TableDictionary::~TableDictionary()
{
}

void TableDictionary::initializeAlphabet()
{
	for (_currentIndexNumber; _currentIndexNumber < _alphabetSize; ++_currentIndexNumber)
	{
		_container.push_back(basic_string<char16_t>{static_cast<uint16_t>(_currentIndexNumber)});
	}
}

bool TableDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index, uint8_t& bitsNumber)
{
	basic_string<char16_t> stringWord(word.begin(), word.end());

	//bitsNumber = static_cast<uint8_t>(log2(ceil(static_cast<double>(_currentIndexNumber))));

	if (!_flushFlag)
	{
		bitsNumber = static_cast<uint8_t>(log2(static_cast<double>(_currentIndexNumber))) + 1;
	}
	else
	{
		bitsNumber = static_cast<uint8_t>(log2(static_cast<double>(_maxIndexNumber))) + 1;
	}

	for(uint32_t i=0; i <_container.size(); ++i)
	{
		if (stringWord == _container[i])
		{
			index = i;
			return true;
		}
	}
	return false;
}

uint32_t TableDictionary::insertEntry(std::vector<uint16_t>& word)
{
	basic_string<char16_t> stringWord(word.begin(), word.end());

	if (_currentIndexNumber <= _maxIndexNumber)
	{
		if (_listOverflow)
		{
			insertElementToList(_currentIndexNumber);
			_container.push_back(stringWord);
			return _currentIndexNumber++;
		}
		else
		{
			_container.push_back(stringWord);
			return _currentIndexNumber++;
		}
	}
	else
	{
		if (_listOverflow)
		{
			return removeElementFromList(stringWord);
		}
		else
		{
			flush();
			//_container.push_back(stringWord);
			return _currentIndexNumber;
		}
	}
}

bool TableDictionary::getEntry(uint32_t index, std::vector<uint16_t>& word)
{
	if (index < _container.size())
	{
		word = vector<uint16_t>{ _container[index].begin(), _container[index].end() };
		return true;
	}
	else
	{
		return false;
	}
}

void TableDictionary::flush()
{
	_container.clear();
	_currentIndexNumber = 0;
	initializeAlphabet();
	//_flushFlag = true;
}

uint32_t TableDictionary::removeElementFromList(std::basic_string<char16_t> stringWord)
{
	auto iter = _indexList.begin();
	uint32_t removedIndex = (*iter).second;

	// Remove from list.
	_indexList.erase(iter);

	// Remove old and add new element to dictionary.
	_container[removedIndex] = stringWord;

	//Add new element to the list.
	_indexList.push_back(make_pair(1, removedIndex));
	
	return removedIndex;
}

void TableDictionary::insertElementToList(uint32_t currenIndex)
{
	_indexList.push_back(make_pair(1, currenIndex));
}

