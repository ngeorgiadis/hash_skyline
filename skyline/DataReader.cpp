#include "stdafx.h"
#include "DataReader.h"

DataReader::DataReader(const char* filename)
{
	_filename = filename;
	_buffer = new char[_buffer_size];
	_f = fopen(_filename, "r");
	size_t n = fread(_buffer, sizeof(char), _buffer_size, _f);

	//if file fits to buffer
	if ( n < _buffer_size) {
		_buffer_size = n;
	}

	for (int i = 0; i < _buffer_size; i++) {

		if (_buffer[_buffer_size - i] == '\n') {
			_buffer[_buffer_size - i+1] = 0x00;
			fseek(_f, -i+1 , SEEK_CUR);
			break;
		}

	}

	_line = strtok_r(_buffer, "\n", &_tok_context);
	_eof = feof(_f) == 1 ? true : false;
}

DataReader::~DataReader()
{
	delete _buffer;
}

char* DataReader::Readline()
{
	if (_first_line) {
		_first_line = false;
		return _line;
	}
	
	_line = strtok_r(nullptr, "\n", &_tok_context);

	if (_line == nullptr && _last_block) {
		_eof = true;
		return "";
	}

	if (_line == nullptr) {

		//need to read again
		size_t n = fread(_buffer, sizeof(char), _buffer_size, _f);

		//the last block just read
		if (n != _buffer_size) {
			_last_block = true;
			_buffer[n] = 0x00;
		}
		else {

			for (int i = 0; i < _buffer_size; i++) {

				if (_buffer[_buffer_size - i] == '\n') {
					_buffer[_buffer_size - i + 1] = 0x00;
					fseek(_f, -i + 1, SEEK_CUR);
					break;
				}

			}

		}
		_line = strtok_r(_buffer, "\n", &_tok_context);
	}
	
	if (_line == nullptr)
		_line = "";

	return _line;
}

vec DataReader::LineToVector(char* line)
{
	vector<double> vector;

	char *context;
	char* str = strtok_r(line, " ", &context);

	while (str != nullptr) {

		vector.push_back(atof(str));
		str = strtok_r(nullptr, " ", &context);

	}

	return vec(vector);
}

vector<float> DataReader::LineToFloatVector(char* line, const char* delimiter  )
{
	vector<float> vector;
	char *context;
	char* str = strtok_r(line, delimiter, &context);
	while (str != nullptr) {
		vector.push_back(atof(str));
		str = strtok_r(nullptr, delimiter, &context);
	}
	return vector;
}

vec DataReader::LineToVector(char* line, const char* delimiter)
{
	vector<double> vector;

	char *context;
	char* str = strtok_r(line, delimiter, &context);
	while (str != nullptr) {

		vector.push_back(atof(str));
		str = strtok_r(nullptr, delimiter, &context);

	}

	return vec(vector);
}