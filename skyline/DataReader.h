#pragma once
class DataReader
{
	bool _eof = false;
	bool _first_line = true;
	bool _last_block = false;

	size_t _buffer_size = 64 * 1024 *1024;
	FILE* _f;
	char* _buffer;
	char* _line;
	char* _tok_context;
	const char* _filename;


public:
	DataReader(const char* filename);
	~DataReader();
	char* Readline();

	static vec LineToVector(char* line);
	static vector<float> LineToFloatVector(char* line, const char* delimiter = " ");
	static vec LineToVector(char* line, const char* delimiter);

	bool eof() {
		return _eof;
	}

};

