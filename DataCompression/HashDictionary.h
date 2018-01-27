#pragma once
#include "Dictionary.h"

#include <unordered_map>
#include <string>

class HashDictionary final : public Dictionary
{
public:
	HashDictionary(uint8_t entryBitSize, uint8_t outputBitSize, bool listOverflow);
	~HashDictionary();

	bool getIndex(std::vector<uint16_t>& word, uint32_t& index, uint8_t& bitsNumber) override;

	uint32_t insertEntry(std::vector<uint16_t>& word) override;
	bool getEntry(uint32_t index, std::vector<uint16_t>& word) override;

	void flush() override;
	uint32_t removeElementFromList(std::basic_string<char16_t>) override;
	virtual void insertElementToList(uint32_t currenIndex) override;
private:
	void initializeAlphabet() override;

	std::unordered_map< std::basic_string<char16_t>, uint32_t > _container;
};

