#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "LZWEngine.h"


int main(int argc, char** argv)
{
	LZWEngine eng;

	try{
	eng.Code("C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\barbara.pgm", "C:\\Users\\Hubert\\Documents\\STUDIA_INFA_MGR\\semestr_1\\KODA\\Projekt\\LZWcoding\\TestOut.lzw");
	}
	catch (std::exception e) {
		std::cout<<e.what()<<std::endl;
	}

	system("PAUSE");

	return 0;
}