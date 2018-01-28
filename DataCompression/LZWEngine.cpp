#include "LZWEngine.h"

std::vector<uint32_t> code_test;
std::vector<uint8_t> data_test;

bool listOverflow = false;

int LZWEngine::Code(const char* source, const char* dest, uint32_t dicSize) {

	_pReader.reset(new DataReader(source));

	_sCompressConfig.overflow_type = WorkingSet;
	_sCompressConfig.dictionary_size = dicSize;
	_symbolBitsNumber.clear();

	_pDictionary.reset(new HashDictionary(8, _sCompressConfig.dictionary_size, listOverflow));


	vector<uint32_t> codeVector;
	auto inVec = _pReader->getBuffer();
	auto inputElement = inVec.begin();

	if (inputElement != inVec.end()) {

		vector<uint16_t> newDictionaryWord{ static_cast<uint16_t>(*inputElement) };
		int nItr = 0;
		while (1)
		{
			nItr++;
			//printf("%d\n",nItr);
			++inputElement;



			if (inputElement == inVec.end())
			{
				uint32_t currentOutputIndex;
				uint8_t currentIndxSize;
				bool wordExistence = _pDictionary->getIndex(newDictionaryWord, currentOutputIndex, currentIndxSize);
				

				if (!wordExistence)
					throw std::logic_error("Can't get entry for the last symbol");

				codeVector.push_back(currentOutputIndex);
				_symbolBitsNumber.push_back(std::pair<uint32_t, unsigned int>(currentOutputIndex, currentIndxSize));

				break;
			}



			newDictionaryWord.push_back(*inputElement);

			uint32_t currentOutputIndex;
			uint8_t currentIndxSize;
			if (!_pDictionary->getIndex(newDictionaryWord, currentOutputIndex, currentIndxSize))
			{
				newDictionaryWord.pop_back();
				bool wordExistence = _pDictionary->getIndex(newDictionaryWord, currentOutputIndex, currentIndxSize);

				if (!wordExistence)
					throw std::logic_error("Can't get entry symbol");
				codeVector.push_back(currentOutputIndex);
				_symbolBitsNumber.push_back(std::pair<uint32_t, unsigned int>(currentOutputIndex, currentIndxSize));

				newDictionaryWord.push_back(*inputElement);
				_pDictionary->insertEntry(newDictionaryWord);

				newDictionaryWord.clear();
				newDictionaryWord.push_back(*inputElement);
			}
		}
	}


	_pCoder.reset(new DataCoder(dest));
	_pCoder->writeCompressionHeader(_sCompressConfig);
	int n = 0;
	for (const auto& i : codeVector)
	{
		_pCoder->writeIndex(i, _symbolBitsNumber[n].second);
		++n;
	}

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

	_pCoder.reset(new DataCoder(dest));
	_pReader.reset(new DataReader(source));


	if (!_pReader->isDecoding())
		return -1;

	auto dictionarySize = _pReader->getConfig().dictionary_size;
	auto owfType = _pReader->getConfig().overflow_type;

	_pDictionary.reset(new TableDictionary(8, dictionarySize, listOverflow));


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
		if (_pDictionary->isFlushState())
		{
			newWord.clear();
			indxSize = _pDictionary->getBitsNumber();
			previousInputElement = _pReader->getSymbol(indxSize);
			newWord.push_back(previousInputElement);
			decompressedVector.insert(decompressedVector.end(), newWord.begin(), newWord.end());
			++n_it;
		}
		indxSize=_pDictionary->getBitsNumber();

		try
		{
			inputElement = _pReader->getSymbol(indxSize);

			// Only for debug
			if (previousInputElement == code_test[n_it-2])
			{
				int a = 4;
			}
			else
			{
				int b = 4;
			}

			

			indexExistence = _pDictionary->getEntry(inputElement, currentWord);
			if (indexExistence)
			{
				decompressedVector.insert(decompressedVector.end(), currentWord.begin(), currentWord.end());
				newWord.push_back(currentWord.front());

				_pDictionary->insertEntry(newWord);
				newWord = currentWord;
			}
			else 
			{
				vector<uint16_t> previousWord;

				indexExistence = _pDictionary->getEntry(previousInputElement, previousWord);
				if (!indexExistence)
					throw std::logic_error("Can't get word for the index in critical situation");
				auto newCriticalWord = previousWord;
				newCriticalWord.push_back(previousWord.front());

				decompressedVector.insert(decompressedVector.end(), newCriticalWord.begin(), newCriticalWord.end());

				_pDictionary->insertEntry(newCriticalWord);
				newWord = newCriticalWord;
			}


		}
		catch (std::out_of_range)
		{
			break;
		}

		
		

	}


	for (const auto& i : decompressedVector)
		_pCoder->writeIndex(i,8);


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
