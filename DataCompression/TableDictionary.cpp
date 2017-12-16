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
		_container.push_back(basic_string<char16_t>{static_cast<uint16_t>(_currentIndexNumber)});
	}
}

bool TableDictionary::getIndex(std::vector<uint16_t>& word, uint32_t& index)
{
	basic_string<char16_t> stringWord(word.begin(), word.end());

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
		_container.push_back(stringWord);
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
}

