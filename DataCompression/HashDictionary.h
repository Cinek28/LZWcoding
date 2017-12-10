#pragma once
#include "Dictionary.h"
#include <map>

class HashDictionary final : public Dictionary
{
public:
	HashDictionary(uint8_t entryBitSize, uint8_t outputBitSize);
	~HashDictionary();

	bool getIndex(std::vector<uint16_t> word, uint32_t& index) override;

	uint32_t insertEntry(std::vector<uint16_t> word) override;
	bool getEntry(uint32_t index, std::vector<uint16_t>& word) override;

private:
	void initializeAlphabet() override;
	void flush() override;

	std::map< std::vector<uint16_t>, uint32_t > _container;

};

