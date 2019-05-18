#include "stdafx.h"
#include "BinaryDataReader.h"


void BinaryDataReader::fill_buffer()
{
	size_t element_count = _buffer_size / (_vector_size_items + 1);
	size_t c = fread(_buffer, _vector_size_bytes, element_count, _f);
}

void BinaryDataReader::init()
{
	//reset 
	fseek(_f, 0, SEEK_SET);

	//read first 4bytes to find the size of vecetor
	size_t c = fread(&_vector_size_items, 4, 1, _f);
	_vector_size_bytes = 1 * _element_size + _vector_size_items * _element_size;

	//total number of vectors
	fseek(_f, 0, SEEK_END);
	size_t filesize = ftell(_f);
	_total_vectors = filesize / _vector_size_bytes;

	//reset 
	fseek(_f, 0, SEEK_SET);

	//allocate buffer array
	_buffer_size = 64 * 1024 * (_vector_size_items + 1);
	_buffer = new float[_buffer_size];

	fill_buffer();
}

BinaryDataReader::BinaryDataReader(const char* filename)
{
	_filename = filename;
	_f = fopen( _filename, "rb");
	_element_size = sizeof(float);
	init();
}

BinaryDataReader::~BinaryDataReader()
{
	delete _buffer;
	fclose(_f);

}

mat BinaryDataReader::ReadRandomData(uword numberOfVectors)
{
	delete _buffer;
	init();

	mat m = mat(numberOfVectors, _vector_size_items);

	uword row = 0;

	for (uword i = 0; i < _total_vectors, row < numberOfVectors; ++i)
	{
		int step = (rand() % (_total_vectors / numberOfVectors)) + 1;
		for (int k = 0; k < step; k++)
		{
			ReadNext();
		}
		m.row(i) = ReadNext().t();
		row++;
	}
	return m;
}

mat BinaryDataReader::ReadRandomData(uword numberOfVectors, map<int, int> *dimension_mapping)
{
	delete _buffer;
	init();

	mat m = mat(numberOfVectors, dimension_mapping->size());

	uword row = 0;

	for (uword i = 0; i < _total_vectors, row < numberOfVectors; ++i)
	{
		int step = (rand() % (_total_vectors / numberOfVectors)) + 1;
		for (int k = 0; k < step; k++)
		{
			ReadNext();
		}

		m.row(i) = ReadNext(dimension_mapping).t();

		row++;
	}
	return m;
}

vector<vector<double>> BinaryDataReader::GetValueCardinality()
{
	delete _buffer;
	init();

	vector<vector<double>> c;

	bool f = true;
	while (!eof())
	{
		//read vector from the dataset
		auto v = ReadNext();

		//first entry is the number of dimension
		if (f) {
			for (int i = 0; i < v.n_rows; i++)
			{
				vector<double> d1;
				d1.push_back(i);
				c.push_back(d1);
			}
			f = false;
		}

		//for each vector read from the dataset
		//for the dimension i find if it contains different value
		//and add it to the output vector
		for (int i = 0; i < v.n_rows; i++)
		{
			if (std::find(c[i].begin(), c[i].end(), v(i)) == c[i].end()) {
				c[i].push_back(v(i));
			}
		}

		if (_count % 10000 == 0)
		{
			cout << ".";
		}
	}
	return c;
}

vec BinaryDataReader::ReadNext()
{
	if (feof(_f) != 0 && _buffer_empty)
	{
		throw new ApplicationException("cannot read past the end of file.");
	}

	_count++;

	float* v = new float[_vector_size_items + 1];

	memcpy(v, _buffer + _index, _vector_size_bytes);

	std::vector<float> v1 = vector<float>(v + 1, v + _vector_size_items + 1);

	_index += (_vector_size_items + 1);
	if (_index > (_buffer_size - _vector_size_items - 1))
	{
		fill_buffer();
		if (feof(_f) != 0)
		{
			_buffer_empty = true;
		}
		_index = 0;
	}

	delete v;
	return arma::conv_to<vec>::from(v1);
}

vec BinaryDataReader::ReadNext(map<int,int>* dimension_mapping)
{
	vec v = ReadNext();

	vec n = vec(dimension_mapping->size());
	for(int i=0;i<dimension_mapping->size();i++)
	{
		n(i) = v(dimension_mapping->at(i));
	}
	return n;
}

bool BinaryDataReader::eof() const
{
	return (feof(_f) != 0) && _buffer_empty;
}
