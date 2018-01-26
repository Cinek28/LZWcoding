#include "LZWEngine.h"

std::vector<uint32_t> code_test;
std::vector<uint8_t> data_test;

int LZWEngine::Code(const char* source, const char* dest, uint8_t indexBitCount) {

	_pReader.reset(new DataReader(source));

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = indexBitCount;
	
	_pDictionary.reset(new HashDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));


	vector<uint32_t> codeVector;
	auto inVec = _pReader->getBuffer();
	auto inputElement = inVec.begin();

	if(inputElement!=inVec.end()){

			vector<uint16_t> newDictionaryWord{ static_cast<uint16_t>(*inputElement) };
				
			while(1)
			{
			
				++inputElement;
				

				if(inputElement==inVec.end())
				{
					uint32_t currentOutputIndex;
					bool wordExistence=_pDictionary->getIndex(newDictionaryWord, currentOutputIndex);
					uint8_t indxSize = 0;
					while (currentOutputIndex)
					{
						++indxSize;
						currentOutputIndex >>= 1;
					}

				
					if(!wordExistence)
						throw std::logic_error("Can't get entry for the last symbol");
					codeVector.push_back(currentOutputIndex);
				
					break;
				}

				
		
				newDictionaryWord.push_back(*inputElement);

				uint32_t currentOutputIndex;
				if (!_pDictionary->getIndex(newDictionaryWord, currentOutputIndex)) 
				{
					newDictionaryWord.pop_back();
					bool wordExistence=_pDictionary->getIndex(newDictionaryWord, currentOutputIndex);				
				
					if(!wordExistence)
						throw std::logic_error("Can't get entry symbol");
					codeVector.push_back(currentOutputIndex);

					newDictionaryWord.push_back(*inputElement);
					auto overflowFlag = _pDictionary->insertEntry(newDictionaryWord);
				
					if (overflowFlag == _pDictionary->getOverflowFlag())
					{
						_pDictionary->insertEntry(newDictionaryWord); // New word is inserted to new dictionary.
					}

					newDictionaryWord.clear();
					newDictionaryWord.push_back(*inputElement);
				}
			}
		}


	_pCoder.reset(new DataCoder(dest, _sCompressConfig.indx_bit_count));
	_pCoder->writeCompressionHeader(_sCompressConfig);
	for (const auto& i : codeVector)
		_pCoder->writeIndex(i);
	

	size_t insize = inVec.size();
	printf("\nCompress succeded\n raw data size=%llu\n compressed data size=%llu\n\n", insize, codeVector.size());
	_pCoder.reset();
	///////////////////
	code_test = codeVector;
	data_test = inVec;
	//////////////////
	return 0;
}


int LZWEngine::Decode(const char* source, const char* dest) {

	_pCoder.reset(new DataCoder(dest, 8));
	_pReader.reset(new DataReader(source));
	

	if (!_pReader->isDecoding())
		return -1;

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = _pReader->getConfig().indx_bit_count;
	_pDictionary.reset(new TableDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));


	//Out vector
	vector<uint16_t> decompressedVector;

	//Words required for LZW
	vector<uint16_t> newWord;
	vector<uint16_t> currentWord;

	//Index size
	uint8_t indxSize = 8;

	//First element from reader
	auto inputElement = _pReader->getSymbol(indxSize);

	bool indexExistence = _pDictionary->getEntry(inputElement, newWord);
	if (!indexExistence)
		throw std::logic_error("Can't get word for the first index");
	decompressedVector.insert(decompressedVector.end(), newWord.begin(), newWord.end());

	int n_it = 1;
	while (1)
			{
				++n_it;
				auto previousInputElement = inputElement;

				try
				{

				while(1)
				{

					inputElement = _pReader->getSymbol(indxSize);

					indexExistence = _pDictionary->getEntry(inputElement, currentWord);
					if (indexExistence) 
					{
						decompressedVector.insert(decompressedVector.end(), currentWord.begin(), currentWord.end());
						newWord.push_back(currentWord.front());

						auto overflowFlag = _pDictionary->insertEntry(newWord);
						if (overflowFlag == _pDictionary->getOverflowFlag())
							_pDictionary->insertEntry(newWord);

						newWord = currentWord;

						break;
					}
					else {

						vector<uint16_t> previousWord;

						indexExistence = _pDictionary->getEntry(previousInputElement, previousWord);
						if (!indexExistence)
							throw std::logic_error("Can't get word for the index in critical situation");

						auto newCriticalWord = previousWord;
						newCriticalWord.push_back(previousWord.front());

						decompressedVector.insert(decompressedVector.end(), newCriticalWord.begin(), newCriticalWord.end());

						auto overflowFlag = _pDictionary->insertEntry(newCriticalWord);
						if (overflowFlag == _pDictionary->getOverflowFlag())
							_pDictionary->insertEntry(newCriticalWord);
						newWord = newCriticalWord;

						break;
				}


					indxSize++;
				}

				}
				catch (std::out_of_range)
				{
					break;
				}

		}
	

	for (const auto& i : decompressedVector)
		_pCoder->writeIndex(i);
	

	printf("\nDecompress succeded\n decompressed data size=%llu\n\n", decompressedVector.size());

	for (int i = 0; i < data_test.size(); ++i)
	{
		if (data_test[i] != decompressedVector[i])
		{
			std::cout << "Aborting because error encountered in file: " << dest << "\r\n";
			printf("\nData element %i not equal; Data val='%u' ; Decoded val='%u'\n", i, data_test[i], decompressedVector[i]);
			return -1;
		}
	}
	return 0;
}

LZWCompressHeader & LZWEngine::getConfig()
{
	return _sCompressConfig;
}
