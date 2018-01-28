#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <random>
#include <time.h>
#include <experimental/filesystem>
#include "LZWEngine.h"
#include "TestUtil.h"

namespace filesystem = std::experimental::filesystem;

int main(int argc, char** argv)
{
	LZWEngine eng;
	TestUtil test;
	ofstream resultFile;
	resultFile.open("test_results_2.csv", ios::out);
	resultFile << test.getCsvHeader();
	for (auto & path : filesystem::directory_iterator(".\\Files"))
	{
		filesystem::path file = filesystem::canonical(path);
		for (int i = 12; i < 27; i=i+2) {
			std::cout << path << std::endl;
			test.runTest(&eng, file.u8string().c_str(), ".\\TestOut.lzw", ".\\result.pgm", i);
			resultFile << test.csvEntry(file.u8string().substr(file.u8string().find_last_of("\\") + 1).c_str());
		}
		test.entropyCalculated = false;
		string filename = file.u8string().substr(file.u8string().find_last_of("\\") + 1).c_str();
		filename = ".\\Histograms\\" + filename.substr(0, filename.find_last_of(".")) + "histogram.csv";
		test.csvHistogramEntry(filename);
	}
	resultFile.close();

	system("PAUSE");

	return 0;
}