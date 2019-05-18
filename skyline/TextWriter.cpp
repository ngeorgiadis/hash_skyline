#include "stdafx.h"
#include "TextWriter.h"


TextWriter::TextWriter(string filename)
{
	_outstream.open(filename);
}

TextWriter::~TextWriter()
{
	_outstream.close();
}

void TextWriter::WriteLine(string text)
{
	_outstream << text << endl;
}

void TextWriter::WriteLine(vector<float> data)
{
	for (auto v: data)
	{
		_outstream << static_cast<int>(v) << "\t";
	}

	_outstream << endl;
}

void TextWriter::Write(string text)
{
	_outstream << text;
}

void TextWriter::Write(vector<float> data)
{
	for (auto v : data)
	{
		_outstream << static_cast<int>(v) << "\t";
	}
}
