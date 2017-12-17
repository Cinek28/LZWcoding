#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <random>
#include <time.h>
#include "LZWEngine.h"

void testCodeDecode()
{
	for (int bits = 8; bits <= 32; ++bits)
	{
		bool fail = false;
		DataCoder * coder = new DataCoder("test.lzw", bits);
		LZWCompressHeader header;
		std::vector<uint32_t> input;
		header.data_order_type = Linear;
		header.indx_bit_count = bits;
		header.word_bit_count = 8;
		coder->writeCompressionHeader(header);
		uint32_t val, count = rand() % 100000 + 5000;
		uint32_t indexMask = 0;
		//Reassemble 8 bit to output bit size//
		for (int i = 0; i < bits; ++i)
		{
			indexMask |= 1 << i;
		}
		for (int i = 0; i < count; ++i)
		{
			val = rand() & indexMask;
			input.push_back(val);
			coder->writeIndex(val);
		}
		delete coder;

		DataReader* reader = new DataReader("test.lzw");
		header = reader->getConfig();
		std::vector<std::vector<uint32_t>> data = reader->getBuffer();
		if (input.size() != data[0].size())
		{
			std::cout << "Invalid size: " << input.size() << " != " << data[0].size() << std::endl;
			fail = true;
		}
		for (int i = 0; i < data[0].size(); ++i)
		{
			if (data[0][i] != input[i])
			{
				std::cout << "Data " << i << " = " << data[0][i] << " - " << input[i] << " - error" << std::endl;
				fail = true;
			}
		}
		delete reader;

		std::cout << "Comparison of " << input.size() << " elements for bit size " << bits << " succesful!\r\n";
		input.clear();
		data.clear();
	}
}
int main(int argc, char** argv)
{
	//LZWEngine eng;

	//try{
	//	eng.Code("C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\barbara.pgm", "C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\TestOut.lzw");
	//	eng.Decode("C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\TestOut.lzw", "C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\finito.pgm");
	//}
	//catch (std::exception e) {
	//	std::cout<<e.what()<<std::endl;
	//}
	srand(time(NULL));
	testCodeDecode();
	system("PAUSE");

	return 0;
}