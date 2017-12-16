#include "LZWEngine.h"

int LZWEngine::Code(const char* source, const char* dest) {

	
	_pReader.reset(new DataReader(source));

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = 14;
	
	_pDictionary.reset(new HashDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));


	vector<uint32_t> codeVector;
	auto FileData = _pReader->getBuffer();

	for(auto& inVec:FileData){

		auto inputElement = inVec.begin();

		if(inputElement!=inVec.end()){

		vector<uint16_t> newDictionaryWord {*inputElement};

		while(1)
		{
			++inputElement;

			if(inputElement==inVec.end()){
				uint32_t currentOutputIndex;
				bool wordExistence=_pDictionary->getIndex(newDictionaryWord, currentOutputIndex);
				
				if(!wordExistence)
					throw std::logic_error("Can't get entry for the last symbol");
				codeVector.push_back(currentOutputIndex);
				
				break;
			}
		
			// TO DO: flush after header
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

		_pDictionary->flush();
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

	return 0;
}


int LZWEngine::Decode(const char* source, const char* dest) {

	////Decode_TEST////

	_pReader.reset(new DataReader(source));

	if (!_pReader->isDecoding())
		return -1;

	_sCompressConfig.data_order_type = _pReader->getConfig().data_order_type;
	_sCompressConfig.word_bit_count = _pReader->getConfig().word_bit_count;
	_sCompressConfig.indx_bit_count = _pReader->getConfig().indx_bit_count;

	_pDictionary.reset(new TableDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));

	auto FileData=_pReader->getBuffer();

	vector<uint16_t> outVec;

	for(auto& inVec:FileData){

		auto i_dec = inVec.begin();

		if (i_dec != inVec.end()) {


			vector<uint16_t> N;
			vector<uint16_t> W;

			bool bresf = _pDictionary->getEntry(*i_dec, N);
			if (!bresf)
				throw std::logic_error("Can't get word for the first index");
			outVec.insert(outVec.end(), N.begin(), N.end());

			int n_it = 1;
			while (1)
			{
				++n_it;
				++i_dec;

				if (i_dec == inVec.end()) {
					break;
				}


				bool res = _pDictionary->getEntry(*i_dec, W);
				if (res) {
					outVec.insert(outVec.end(), W.begin(), W.end());
					N.push_back(W.front());
					auto ovf = _pDictionary->insertEntry(N);
					if (ovf == ~(0 << 31))
						_pDictionary->insertEntry(N);

					N = W;
				}
				else {

					auto i_tmp = i_dec - 1;
					vector<uint16_t> W_tmp;

					bool res = _pDictionary->getEntry(*i_tmp, W_tmp);
					if (!res)
						throw std::logic_error("Can't get word for the index in critical situation");

					auto W_new = W_tmp;
					W_new.push_back(W_tmp.front());

					outVec.insert(outVec.end(), W_new.begin(), W_new.end());

					auto ovf = _pDictionary->insertEntry(W_new);
					if (ovf == ~(0 << 31))
						_pDictionary->insertEntry(W_new);
					N = W_new;
				}
		}
	}
	}



	size_t insize = 0;
	for (const auto& v : FileData) {
		insize += v.size();
	}
	printf("\nDecompress succeded\n compressed data size=%llu\n decompressed data size=%llu\n\n", insize, outVec.size());


	return 0;
}