#pragma once
class FvecWriter
{
	FILE* _f;
public:
	FvecWriter(FILE* f);
	FvecWriter(string file, bool append = false);
	~FvecWriter();


	void Write(void* data, int size) const;
	void Write(vector<float> data) const;
};

