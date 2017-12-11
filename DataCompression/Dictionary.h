#pragma once

#include <vector>
#include <cstdint>

class Dictionary
{
public:
	Dictionary(uint8_t entryBitSize, uint8_t outputBitSize);
	virtual ~Dictionary();

	virtual bool getIndex(std::vector<uint16_t>& word, uint32_t& index) = 0;

	virtual uint32_t insertEntry(std::vector<uint16_t>& word) = 0;
	virtual bool getEntry(uint32_t index, std::vector<uint16_t>& word) = 0;

	uint32_t getOverflowFlag();

protected:
	virtual void initializeAlphabet() = 0;
	virtual void flush() = 0;

	void initializeValues();

	uint8_t _entryBitSize;
	uint8_t _outputBitSize;

	uint16_t _alphabetSize;
	uint32_t _currentIndexNumber;
	uint32_t _maxIndexNumber;

	const uint16_t FLAGS_NUMBER = 1;
	uint16_t _overflowFlag;
};

