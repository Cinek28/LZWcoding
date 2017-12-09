#pragma once

#include <map>
#include <vector>
#include <cstdint>

class Dictionary
{
public:
	Dictionary(uint8_t entryBitSize, uint8_t outputBitSize);
	~Dictionary();

	bool getEntry(std::vector<uint16_t> word, uint32_t& index);
	std::uint32_t insertEntry(std::vector<uint16_t> word);

private:
	void initializeAlphabet();

	uint16_t _alphabetSize;
	uint32_t _currentIndexNumber;
	uint32_t _maxIndexNumber;

	std::map<std::vector<uint16_t>, uint32_t> _container;
};

