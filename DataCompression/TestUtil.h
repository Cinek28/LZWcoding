#pragma once
class TestUtil
{
public:
	TestUtil();
	~TestUtil();

	double getEntropy();

	double getCompressionRate();

	static double calculateTime(std::function<void()> fun);

	void reset();


private:     
	double compressionRate;
	double entropy;
	double codingTime;
    double decodingTime;
};

