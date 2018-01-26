#pragma once

#include <vector>
#include <cstdint>
#include <list>
#include <utility>

class Dictionary
{
public:
	Dictionary(uint8_t entryBitSize, uint8_t outputBitSize);
	virtual ~Dictionary();

	virtual bool getIndex(std::vector<uint16_t>& word, uint32_t& index, uint8_t& bitsNumber) = 0;

	virtual uint32_t insertEntry(std::vector<uint16_t>& word) = 0;
	virtual bool getEntry(uint32_t index, std::vector<uint16_t>& word) = 0;

	uint32_t getOverflowFlag();
	virtual void flush() = 0;
	virtual uint32_t removeElementFromList(std::basic_string<char16_t>) = 0;
	virtual void insertElementToList(uint32_t currenIndex) = 0;

	uint8_t getBitsNumber();

protected:
	virtual void initializeAlphabet() = 0;

	void initializeValues();

	uint8_t _entryBitSize;
	uint8_t _outputBitSize;

	uint16_t _alphabetSize;
	uint32_t _currentIndexNumber;
	uint32_t _maxIndexNumber;

	const uint16_t FLAGS_NUMBER = 1;
	uint16_t _overflowFlag;

	std::list<std::pair<uint32_t, uint32_t> > _indexList;
};

