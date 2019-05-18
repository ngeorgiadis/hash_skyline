#include "stdafx.h"
#include "FvecReader.h"


FvecReader::FvecReader(string filename)
{
	fopen_s(&f, filename.c_str(), "rb");
}

FvecReader::~FvecReader()
{
	fclose(f);
}

bool FvecReader::Read(void* data, int size)
{
	size_t res = fread(data, size, 1, f);
	if (res != 1)
		return false;
	return true;
}

bool FvecReader::ReadNext(vector<float> &data) 
{
	int size = -1;
	size_t res = fread(reinterpret_cast<void*>(&size), sizeof(int), 1, f);

	if (res != 1)
		return false;


	if (size > 1024 * 100)
		throw ApplicationException("Possible error. vector size too large");

	float* value = new float[size];
	fread(reinterpret_cast<void*>(&value[0]), sizeof(float), size, f);
	
	data = vector<float>(value, value + size);
	delete value;

	return true;
}

bool FvecReader::ReadNext(vector<double> &data)
{
	int size = -1;
	size_t res = fread(reinterpret_cast<void*>(&size), sizeof(int), 1, f);

	if (res != 1)
		return false;


	if (size > 1024 * 100)
		throw ApplicationException("Possible error. vector size too large");

	float* value = new float[size];
	fread(reinterpret_cast<void*>(&value[0]), sizeof(float), size, f);

	data = vector<double>(value, value + size);
	delete value;

	return true;
}

bool FvecReader::ReadNext(vector<float>& data, map<int, int>* dimension_mapping)
{
	vector<float> v;
	if (ReadNext(v))
	{
		vec n = vec(dimension_mapping->size());
		for (int i = 0; i<dimension_mapping->size(); i++)
		{
			data.push_back(v[dimension_mapping->at(i)]);
		}

		return true;
	}

	return false;
}

bool FvecReader::eof() const
{
	return feof(f);
}
