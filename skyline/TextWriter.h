#pragma once
class TextWriter
{
	ofstream _outstream;
public:
	TextWriter(string filename);
	~TextWriter();

	void Write(string text);
	void Write(vector<float> data);

	void WriteLine(string text);
	void WriteLine(vector<float> data);
};

