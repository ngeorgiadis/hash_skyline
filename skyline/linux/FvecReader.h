#pragma once
class FvecReader
{
	FILE* f;
public:
	FvecReader(string filename);
	~FvecReader();

	bool Read(void* data, int size);

	bool ReadNext(vector<float> &data);
	bool ReadNext(vector<double> &data);
	bool ReadNext(vector<float> &data, map<int, int>* dimension_mapping);
	bool eof() const;
};

