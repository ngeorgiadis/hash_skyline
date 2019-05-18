#include "stdafx.h"
#include "FvecWriter.h"


FvecWriter::FvecWriter(FILE* f)
{
	_f = f;
}

FvecWriter::FvecWriter(string file, bool append)
{
	string mode = append ? "ab" : "wb";
	fopen_s(&_f, file.c_str(), mode.c_str());
}

FvecWriter::~FvecWriter()
{
	fclose(_f);
}

void FvecWriter::Write(void* data, int size) const
{
	fwrite(data, size, 1, _f);
}


void FvecWriter::Write(vector<float> data) const
{
	int size = data.size();
	fwrite(reinterpret_cast<void*>(&size), sizeof(int), 1, _f);

	float* buffer = reinterpret_cast<float*>(&data[0]);
	fwrite(buffer, sizeof(float), size, _f);
}
