#pragma once

#include "Dictionary.h"


class TableDictionary final : public Dictionary
{
public:
	TableDictionary(uint8_t entryBitSize, uint8_t outputBitSize);
	~TableDictionary();

	bool getIndex(std::vector<uint16_t> word, uint32_t& index) override;

	uint32_t insertEntry(std::vector<uint16_t> word) override;
	bool getEntry(uint32_t index, std::vector<uint16_t>& word) override;

private:
	void initializeAlphabet() override;
	void flush() override;

	std::vector<std::vector<uint16_t>> _container;
};

