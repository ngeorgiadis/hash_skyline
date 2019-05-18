#pragma once
class BinaryDataReader
{
	const char* _filename;
	FILE* _f;
	size_t _buffer_size;
	size_t _element_size;
	size_t _total_vectors;
	size_t _vector_size_bytes;
	size_t _index = 0;

	size_t _file_position;

	float* _buffer;
	int _vector_size_items;
	uword _n_rows;
	uword _n_features;
	bool _eof;
	bool _buffer_empty = false;

	void fill_buffer();
	void init();
	size_t _count = 0;


public:
	BinaryDataReader(const char* filename);
	~BinaryDataReader();

	mat ReadRandomData(uword numberOfVectors);
	mat ReadRandomData(uword numberOfVectors, map<int, int> *dimension_mapping);
	vector<vector<double>> GetValueCardinality();
	vec ReadNext();
	vec ReadNext(map<int, int>* dimension_mapping);
	bool eof() const;

	 
};

