#include "stdafx.h"
#include "Common.h"
#include "FvecReader.h"


Common::Common()
{
}

Common::~Common()
{
}

string Common::VectorToString(vec v)
{
	string s = "";
	for(int i =0;i<v.n_rows;i++)
	{
		
		if (i < v.n_rows-1)
			s.append(std::to_string(static_cast<unsigned long>(v[i])) + " ");
		else
			s.append(std::to_string(static_cast<unsigned long>(v[i])));
	}
	return s;
}

string Common::VectorToString(rowvec v)
{
	string s = "";
	for (int i = 0; i<v.n_cols; i++)
	{

		if (i < v.n_cols - 1)
			s.append(std::to_string(static_cast<unsigned long>(v[i])) + " ");
		else
			s.append(std::to_string(static_cast<unsigned long>(v[i])));
	}
	return s;
}

string Common::CompressVector(vec v)
{
	string s = "";

	for (uword i = 0; i < v.n_rows; i++) {

		if (v[i] > 0) {

			s.append("1");

		}
		else {

			s.append("0");

		}
	}

	return s;
}

string Common::CompressRowVector(rowvec v)
{

	string s = "";

	for (uword i = 0; i < v.n_cols; i++) {
	
		if (v[i] > 0) {

			s.append("1");

		}
		else {

			s.append("0");

		}
	}

	return s;
}

uword Common::CompressRowVectorToInt(rowvec v)
{
	
	uword res = 0;
	for (uword i = 0; i < v.n_cols; i++) {

		if (v[i] > 0)
			res += pow(2, i);

	}
	return res;
}

mat Common::MeanNormalization(mat data)
{
	vec means = vec(data.n_cols);
	means.fill(0);

	for (uword c = 0; c < data.n_cols; c++) {
		double sum = 0;
		for (uword r = 0; r < data.n_rows; r++) {
			sum += data(r, c);
		}
		means(c) = sum / data.n_rows;
	}

	for (uword c = 0; c < data.n_cols; c++) {
		for (uword r = 0; r < data.n_rows; r++) {
			data(r, c) = data(r, c) - means(c);
		}
	}
	return data;
}

uvec Common::RandomPermutation(uword n)
{
	std::vector<uword> v;
	for (uword i = 0; i < n; i++) {
		v.push_back(i);
	}
	std::random_shuffle(v.begin(), v.end());
	return uvec(v);
}

mat Common::Convert(umat data)
{
	mat res = mat(data.n_rows, data.n_cols);
	for (uword row = 0; row < data.n_rows; ++row) {
		for (uword col = 0; col < data.n_cols; ++col) {
			res(row, col) = data(row, col);
		}
	}
	return res;
}

void Common::DecodeFvecBinary(string input, string output)
{
	ofstream outTextFile;
	outTextFile.open(output);

	FILE* f;
	fopen_s(&f, input.c_str(), "rb");

	int line_index = 0;
	while (!feof(f))
	{

		uword size = 0;
		fread(reinterpret_cast<void*>(&size), sizeof(uword), 1, f);

		double* v1 = new double[size];
		fread(reinterpret_cast<void*>(&v1[0]), sizeof(double), size, f);

		outTextFile << line_index << "\t";
		for (int i = 0; i<size; i++)
		{
			outTextFile << v1[i] << " ";
		}

		outTextFile << endl;
		line_index++;
	}

	outTextFile.close();
	fclose(f);
}

void Common::DecodeNativeBinary(string input, string output)
{
	ofstream outTextFile;
	outTextFile.open(output);

	FILE* f;
	fopen_s(&f, input.c_str(), "rb");

	int line_index = 0;
	while (!feof(f))
	{

		uword key = 0;
		fread(reinterpret_cast<void*>(&key), sizeof(uword), 1, f);

		size_t num_of_vectors = 0;
		fread(reinterpret_cast<void*>(&num_of_vectors), sizeof(size_t), 1, f);

		for (int i = 0; i<num_of_vectors; i++)
		{

			uword size = 0;
			fread(reinterpret_cast<void*>(&size), sizeof(uword), 1, f);

			double* v1 = new double[size];
			fread(reinterpret_cast<void*>(&v1[0]), sizeof(double), size, f);

			outTextFile << line_index << "\t";
			for (int vi = 0; vi<size; vi++)
			{
				outTextFile << v1[vi] << " ";
			}

			outTextFile << endl;
			line_index++;
		}
	}

	outTextFile.close();
	fclose(f);
}

vector<vector<double>> Common::GetValueCardinality(string fvecfilename)
{
	FvecReader reader(fvecfilename);
	vector<vector<double>> c;

	int count = 0;
	bool f = true;
	while (!reader.eof())
	{
		//read vector from the dataset
		vector<float> data;
		if (reader.ReadNext(data))
		{
			count++;

			//first entry is the index number of the dimension
			if (f) {
				for (int i = 0; i < data.size(); i++)
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
			for (int i = 0; i < data.size(); i++)
			{

				auto d = Common::RoundFloat(data[i], 3);

				if (std::find(c[i].begin(), c[i].end(), d) == c[i].end()) {
					c[i].push_back(d);
				}
			}

			if (count % 10000 == 0)
			{
				cout << ".";
			}
		}
	}

	sort(c.begin(), c.end(), [](vector<double> a, vector<double> b)
	{
		if (a.size() == b.size())
		{
			return a[0] < b[0];
		}
		return a.size() > b.size();
	});
	
	return c;
}

void Common::CountVectorsWithZeroValue(string fvecfilename)
{
	FvecReader* reader = new FvecReader(fvecfilename);
	uword vectors_with_zero_values = 0;
	while(!reader->eof())
	{
		vector<float> data;
		if (reader->ReadNext(data))
		{
			auto res = std::find(data.begin(), data.end(), 0);

			if (res != data.end())
			{
				vectors_with_zero_values++;
			}
		}
	}
	cout << "total vectors with zero element : " << vectors_with_zero_values <<endl;
	delete reader;
}

float Common::RoundFloat(float num, int precision)
{
	return round(num * pow(10.0f, precision));
}

void Common::EncodeToFvec(string textfile, string outbin)
{
	DataReader* reader = new DataReader(textfile.c_str());

	ofstream binfile;
	binfile.open(outbin, ios::out | ios::binary);

	int i = 0;

	while (!reader->eof())
	{
		char* line = reader->Readline();
		if (line == "")
		{
			continue;
		}

		auto v = DataReader::LineToFloatVector(line);
		if (v.size() == 0)
		{
			continue;
		}
		v.erase(v.begin());

		int vector_size = v.size();

		binfile.write(reinterpret_cast<char*>(&vector_size), sizeof(int));
		binfile.write(reinterpret_cast<char*>(&v[0]), v.size() * sizeof(float));

		i++;
	}

	cout << "total : " << i << endl;
	delete reader;
	binfile.close();

}

mat Common::GetTrainingData(string fvecfilename, int numberOfRows, int dimensions, map<int,int>* mapping)
{
	mat d(numberOfRows, dimensions);

	FvecReader* reader = new FvecReader(fvecfilename);
	uword line_counter = 0;
	
	if (mapping == nullptr) 
	{
		while (!reader->eof() && line_counter < numberOfRows)
		{
			vector<float> data;
			if (reader->ReadNext(data))
			{
				d.row(line_counter) = conv_to<rowvec>::from(data);
				line_counter++;
			}
		}
	}
	else
	{
		while (!reader->eof() && line_counter < numberOfRows)
		{
			vector<float> data;
			if (reader->ReadNext(data,mapping))
			{
				d.row(line_counter) = conv_to<rowvec>::from(data);
				line_counter++;
			}
		}
	}
	delete reader;
	return d;
}



