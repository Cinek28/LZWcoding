#include "LZWEngine.h"

int LZWEngine::Code(const char* source, const char* dest) {

	
	_pReader.reset(new DataReader(source));

	_sCompressConfig.data_order_type = _pReader->getType().type;
	_sCompressConfig.word_bit_count = _pReader->getType().bit_count;
	_sCompressConfig.indx_bit_count = 2 * _pReader->getType().bit_count;


	auto inVec = _pReader->getBuffer();

	_pDictionary.reset(new HashDictionary(_sCompressConfig.word_bit_count, _sCompressConfig.indx_bit_count));

	vector<uint32_t> outVec;
	auto i = inVec.begin();

	if(i!=inVec.end()){

		vector<uint16_t> w {*i};

		while(1)
		{
			++i;

			if(i==inVec.end()){
				uint32_t indx;
				auto bRes=_pDictionary->getIndex(w, indx);
				bRes == true ? outVec.push_back(indx) : throw std::logic_error("Can't get entry for the last symbol");
				break;
			}
		
			w.push_back(*i);

			uint32_t indx;
			if (!_pDictionary->getIndex(w, indx)) 
			{
				w.pop_back();
				auto bRes=_pDictionary->getIndex(w, indx);
				bRes == true ? outVec.push_back(indx) : throw std::logic_error("Can't get entry symbol");

				w.push_back(*i);
				auto retOvf = _pDictionary->insertEntry(w);
				if (retOvf==~(0 << 31)){
					_pDictionary->insertEntry(w);
				}

				w.clear();
				w.push_back(*i);
			}


		}
	}


	_pCoder.reset(new DataCoder(dest, _sCompressConfig.indx_bit_count));

	for (const auto& i : outVec)
		_pCoder->writeIndex(i);


	return 0;
}


int LZWEngine::Decode(const char* source, const char* dest) {

	////Decode_TEST////
	/*unique_ptr<TableDictionary> tmp_d(new TableDictionary(inType.bit_count, out_bit_size));

	vector<uint16_t> outVec_decode;
	auto i_dec = outVec.begin();

	if (i_dec != outVec.end()) {


		vector<uint16_t> N;
		vector<uint16_t> W;

		bool bresf = tmp_d->getEntry(*i_dec, N);
		if (!bresf)
			throw std::logic_error("Can't get word for the first index");
		outVec_decode.insert(outVec_decode.begin(), N.begin(), N.end());

		int n_it = 1;
		while (1)
		{
			++n_it;
			++i_dec;

			if (i_dec == outVec.end()) {
				break;
			}


			bool res = tmp_d->getEntry(*i_dec, W);
			if (res) {
				outVec_decode.insert(outVec_decode.end(), W.begin(), W.end());
				N.push_back(W.front());
				auto ovf = tmp_d->insertEntry(N);
				if (ovf == ~(0 << 31))
					tmp_d->insertEntry(N);

				N = W;
			}
			else {

				auto i_tmp = i_dec - 1;
				vector<uint16_t> W_tmp;

				bool res = tmp_d->getEntry(*i_tmp, W_tmp);
				if (!res)
					throw std::logic_error("Can't get word for the index in critical situation");

				auto W_new = W_tmp;
				W_new.push_back(W_tmp.front());

				outVec_decode.insert(outVec_decode.end(), W_new.begin(), W_new.end());

				auto ovf = tmp_d->insertEntry(W_new);
				if (ovf == ~(0 << 31))
					tmp_d->insertEntry(W_new);
				N = W_new;
			}
		}
	}

	for (int i = 0; i < inVec.size(); ++i) {
		if (inVec[i] != outVec_decode[i])
			std::cout << "Element N*" << i << " not equal \n";
	}*/
	//////////////////

	return 0;
}