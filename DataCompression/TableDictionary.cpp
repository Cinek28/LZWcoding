#include "TableDictionary.h"

using namespace std;

TableDictionary::TableDictionary(uint8_t entryBitSize, uint8_t outputBitSize)
	: Dictionary(entryBitSize, outputBitSize)
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
		_container.push_back(vector<uint16_t>{static_cast<uint16_t>(_currentIndexNumber)});
	}
}

bool TableDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index)
{
	for(uint32_t i=0; i <_container.size(); ++i)
	{
		if (word == _container[i])
		{
			index = i;
			return true;
		}
	}
	return false;
}

uint32_t TableDictionary::insertEntry(std::vector<uint16_t>& word)
{
	if (_currentIndexNumber <= _maxIndexNumber)
	{
		_container.push_back(word);
		return _currentIndexNumber++;
	}
	else
	{
		flush();
		return _overflowFlag;
	}
}

bool TableDictionary::getEntry(uint32_t index, std::vector<uint16_t>& word)
{
	if (index < _container.size())
	{
		word = _container[index];
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
}

