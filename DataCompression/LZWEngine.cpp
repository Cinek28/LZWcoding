#include "LZWEngine.h"

//std::vector<uint32_t> code_test;
//std::vector<uint32_t> data_test;

int LZWEngine::Code(const char* source, const char* dest) {

	
	_pReader.reset(new DataReader(source));

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = 14;
	
	_pDictionary.reset(new HashDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));


	vector<uint32_t> codeVector;
	auto FileData = _pReader->getBuffer();

	vector<uint16_t> newDictionaryWord;

	bool tempFlag = false;
	int dataBlockCounter = 0;

	for(auto& inVec:FileData)
	{
			dataBlockCounter++;
			auto inputElement = inVec.begin();

			if(inputElement!=inVec.end()){


				if (dataBlockCounter==1)
				{
					newDictionaryWord.push_back(static_cast<uint16_t>(*inputElement));
				}
				else 
				{
					tempFlag = true;
				}

			while(1)
			{
				if(!tempFlag)
					++inputElement;
				tempFlag = false;

				if(inputElement==inVec.end() && dataBlockCounter==FileData.size())
				{
					uint32_t currentOutputIndex;
					bool wordExistence=_pDictionary->getIndex(newDictionaryWord, currentOutputIndex);
				
					if(!wordExistence)
						throw std::logic_error("Can't get entry for the last symbol");
					codeVector.push_back(currentOutputIndex);
				
					break;
				}

				if (inputElement == inVec.end())
				{
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
				
					if (overflowFlag == _pDictionary->getOverflowFlag()){
						_pDictionary->insertEntry(newDictionaryWord); // New word is inserted to new dictionary.
					}

					newDictionaryWord.clear();
					newDictionaryWord.push_back(*inputElement);
				}
			}
		}
		//_pDictionary->flush();
	}

	_pCoder.reset(new DataCoder(dest, _sCompressConfig.indx_bit_count));
	_pCoder->writeCompressionHeader(_sCompressConfig);
	for (const auto& i : codeVector)
		_pCoder->writeIndex(i);
	

	size_t insize = 0;
	for (const auto& v : FileData) {
		insize += v.size();
	}
	printf("\nCompress succeded\n raw data size=%llu\n compressed data size=%llu\n\n", insize, codeVector.size());

	///////////////////
	//code_test = codeVector;
	//data_test = FileData[0];
	//data_test.insert(data_test.end(), FileData[1].begin(), FileData[1].end());
	//////////////////
	return 0;
}


int LZWEngine::Decode(const char* source, const char* dest) {

	_pReader.reset(new DataReader(source));

	if (!_pReader->isDecoding())
		return -1;

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = _pReader->getConfig().indx_bit_count;

	_pDictionary.reset(new TableDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));

	auto FileData=_pReader->getBuffer();

	vector<uint16_t> decompressedVector;

	for(auto& inVec:FileData){
		////////////////////////
		/*inVec = code_test;*/
		////////////////////////
		auto inputElement = inVec.begin();

		if (inputElement != inVec.end()) {


			vector<uint16_t> newWord;
			vector<uint16_t> currentWord;

			bool indexExistence = _pDictionary->getEntry(*inputElement, newWord);
			if (!indexExistence)
				throw std::logic_error("Can't get word for the first index");
			decompressedVector.insert(decompressedVector.end(), newWord.begin(), newWord.end());

			int n_it = 1;
			while (1)
			{
				++n_it;
				++inputElement;

				if (inputElement == inVec.end()) {
					break;
				}


				indexExistence = _pDictionary->getEntry(*inputElement, currentWord);
				if (indexExistence) {
					decompressedVector.insert(decompressedVector.end(), currentWord.begin(), currentWord.end());
					newWord.push_back(currentWord.front());

					auto overflowFlag = _pDictionary->insertEntry(newWord);
					if (overflowFlag == _pDictionary->getOverflowFlag())
						_pDictionary->insertEntry(newWord);

					newWord = currentWord;
				}
				else {

					auto previousInputElement = inputElement - 1;
					vector<uint16_t> previousWord;

					indexExistence = _pDictionary->getEntry(*previousInputElement, previousWord);
					if (!indexExistence)
						throw std::logic_error("Can't get word for the index in critical situation");

					auto newCriticalWord = previousWord;
					newCriticalWord.push_back(previousWord.front());

					decompressedVector.insert(decompressedVector.end(), newCriticalWord.begin(), newCriticalWord.end());

					auto overflowFlag = _pDictionary->insertEntry(newCriticalWord);
					if (overflowFlag == _pDictionary->getOverflowFlag())
						_pDictionary->insertEntry(newCriticalWord);
					newWord = newCriticalWord;
				}
		}
	}
	}



	//size_t insize = 0;
	//for (const auto& v : FileData) {
	//	insize += v.size();
	//}
	//printf("\nDecompress succeded\n compressed data size=%llu\n decompressed data size=%llu\n\n", insize, decompressedVector.size());

	//for (int i = 0; i < data_test.size(); ++i)
	//{
	//	if (data_test[i] != decompressedVector[i])
	//		printf("\nData element %i not equal; Data val='%u' ; Decoded val='%u'\n", i, data_test[i], decompressedVector[i]);
	//}

	return 0;
}