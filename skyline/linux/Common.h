#pragma once
class Common
{
public:
	Common();
	~Common();

	static string CompressVector(vec v);
	static string CompressRowVector(rowvec v);
	static uword CompressRowVectorToInt(rowvec v);
	static mat MeanNormalization(mat data);
	static uvec RandomPermutation(uword n);
	static string VectorToString(vec v);
	static string VectorToString(rowvec v);

	static mat Convert(umat data);

	static void DecodeFvecBinary(string input, string output);
	static void DecodeNativeBinary(string input, string output);

	static vector<vector<double>> GetValueCardinality(string fvecfilename);
	static void CountVectorsWithZeroValue(string fvecfilename);
	static float RoundFloat(float number, int nth);
	// input file format
	// linenum <vector> 
	// 1 3 2 3 4 5...N \n
	// 2
	// 3
	static void EncodeToFvec(string textfile, string outbin);
	static mat GetTrainingData(string fvecfilename, int numberOfRows, int dimensions, map<int,int>* mapping = nullptr);

};

