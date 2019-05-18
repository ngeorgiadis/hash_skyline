// skyline.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "FvecReader.h"
#include "FvecWriter.h"
#include "TextWriter.h"

#include "LSH.h"
#include "ITQ.h"
#include "DSH.h"
#include "SphericalHashing.h"

#include <list>
#include <set>
#include <boost/filesystem.hpp>
#include <boost/random.hpp>

#define SKYLINE_BUCKET 0;

float sum(vector<float> v)
{
	float s = 0.0f;
	for (auto i : v)
		s += i;

	return s;
}

class DataPoint
{
	vector<float> _data;
public:
	explicit DataPoint(int ID)
	{
		id = ID;
	}

	int id;
	float minDist = FLT_MAX;
	string key;

	// 0 : leaf
	// 1 : intermediate point
	char type = 0;

	vector<float> Data() const
	{
		return _data;
	}
	
	void SetData(vector<float> data)
	{
		_data = data;
		minDist = sum(data);
	}

	string GetData() const
	{
		string s = "";
		for(int i=0;i<_data.size();i++)
		{
			s += to_string(_data[i]) + "\t";
		}
		return s;
	}
};

struct mindistComparer
{
	bool operator()(const DataPoint& a, const DataPoint& b) const {
		return a.minDist > b.minDist;
	}
};

struct sortComparerStruct
{
	bool operator()(const DataPoint& a, const DataPoint& b) const {
		return a.minDist < b.minDist;
	}
};

typedef map<DataPoint, vector<DataPoint>, mindistComparer>  MindistHeap;
typedef map<string, multiset<DataPoint, sortComparerStruct>> Buckets;
typedef vector<vector<float>> BNLSkyline;

#if SKYLINE_BUCKET == 1
typedef map<string, multiset<DataPoint, sortComparerStruct>> Skyline;
#else
typedef multiset<DataPoint, sortComparerStruct> Skyline;
#endif

Logger logger;
bool useMemoryOnly = false;
bool includeHeapContent = false;
bool useBNL = false;
bool justHashing = false;
int zero_vectors_count = 0;
bool output_identical_skyilne_object = false;

void log_skyline(Skyline skyline)
{
	stringstream outs;
	outs << "skyline" << endl;
	logger.log(&outs);

	
	outs << "id" << "\t" << "minDist" << "\t" << "key" << "\t" << "data" << endl;

#if SKYLINE_BUCKET == 1
	int ssize = 0;
	for (auto bi = skyline.begin(), bend = skyline.end(); bi != bend; ++bi)
	{
		auto skyline_bucket = (*bi).second;
		ssize += skyline_bucket.size();
		for (auto si = skyline_bucket.begin(), send = skyline_bucket.end(); si != send; ++si)
		{
			auto p = (*si);
			outs << p.id << "\t" << p.minDist << "\t" << p.key << "\t" << p.GetData() << endl;
			logger.log(&outs, true);
		}
	}
	logger.log(&outs, true);
	outs << "total skyline items :" << ssize << endl;
	logger.log(&outs);
#else
	for (auto si = skyline.begin(), send = skyline.end(); si != send; ++si)
	{
		auto p = (*si);
		outs << p.id << "\t" << p.minDist << "\t" << p.key << "\t" << p.GetData() <<endl;
	}
	logger.log(&outs, true);
	outs << "total skyline items :" << skyline.size() << endl;
	logger.log(&outs);
#endif
}

HashingBase* GetHashing(string algoritm, string trainFile, int training_length, int dimensions, int encodingLength, map<int,int>* mapping = nullptr)
{

	HashingBase* hash;
	if (algoritm == "lsh")
	{
		hash = new LSH(dimensions, encodingLength);
	}
	else if (algoritm == "itq")
	{
		//get training data
		mat trainITQ = Common::GetTrainingData(trainFile, training_length, dimensions, mapping);
		hash = new ITQ(trainITQ, encodingLength);
	}
	else if (algoritm == "dsh")
	{
		//get training data
		mat trainDSH = Common::GetTrainingData(trainFile, training_length, dimensions, mapping);
		hash = new DSH(trainDSH, encodingLength);
	}
	else if (algoritm == "sph")
	{
		//get training data
		mat trainSPH = Common::GetTrainingData(trainFile, training_length, dimensions, mapping);
		hash = new SphericalHashing(trainSPH, encodingLength);
	}
	else
	{
		throw ApplicationException("ERROR. No hash algorithm selected.");
	}

	return hash;
}

void sort_files(Buckets buckets, string temp_path)
{
	for (auto b : buckets)
	{
		FvecReader* reader = new FvecReader( temp_path + "/" + b.first);
		list<DataPoint> matrix;
		while(!reader->eof())
		{
			vector<float> data;

			int id = -1;
			if (reader->Read(reinterpret_cast<void*>(&id), sizeof(int)))
			{
				DataPoint d(id);
				if (reader->ReadNext(data))
				{
					d.SetData(data);
					matrix.push_back(d);
				}
			}
		}
		delete reader;

		matrix.sort([](const DataPoint&first, const DataPoint&second)
		{
			return first.minDist < second.minDist;
		});

		boost::filesystem::remove(temp_path + "/" + b.first.c_str());

		//write back
		FvecWriter* writer = new FvecWriter( temp_path+"/" + b.first + ".bin");
		for(auto m = matrix.begin(); m != matrix.end(); ++m)
		{
			DataPoint point = (*m);
			writer->Write(&point.id, sizeof(int));
			writer->Write(point.Data());
		}
		delete writer;
	}

}

void logPoint(DataPoint p)
{
	stringstream outs;
	outs << p.id << "\t" << p.minDist << "\t" << p.key << "\t" << p.GetData() << endl;
	logger.log(&outs);
}

void bnl_skyline(string inputFile, map<int, int>* mapping = nullptr, bool ignore_zero_distance = false)
{
	stringstream outs;
	wall_clock clock;
	clock.tic();

	bool useMapping = mapping != nullptr;

	FvecReader* reader = new FvecReader(inputFile);
	BNLSkyline skyline;
	double time_point = clock.toc();
	uword total_read = 0;

	while(!reader->eof())
	{
		total_read++;
		vector<float> data;

		bool res = useMapping ? reader->ReadNext(data, mapping) : reader->ReadNext(data);

		auto distance = sum(data);
		if (res && distance == 0 && ignore_zero_distance) {
			zero_vectors_count++;
			continue;
		}

		if (res)
		{
			if (skyline.size() == 0) {
				skyline.push_back(data);
				continue;
			}

			vec candidate  = conv_to<vec>::from(data);
			list<vector<vector<float>>::iterator> items_to_erase;
			
			bool add_point = false;

			for (auto si = skyline.begin() ; si != skyline.end(); )
			{
				vec skyline_point = conv_to<vec>::from(*si);
				auto sumComp = sum(skyline_point <= candidate);

				//skyline_point is dominating candidate
				if (sumComp == skyline_point.size())
				{
					add_point = false;
					break;
				}

				auto sum2 = sum( skyline_point < candidate);
				if (sum2 == 0)
				{
					//skyline_point should be removed
					//items_to_erase.push_back(si);
					si = skyline.erase(si);
				}
				else 
				{
					++si;
					add_point = true;
				}
			}

			if (add_point) {
				skyline.push_back(data);
			}
		}

		if (clock.toc() - time_point > 5)
		{
			cout << "[" << clock.toc() << " sec], Total items read : " << total_read <<", Skyline size : "<< skyline.size() << endl;
			time_point = clock.toc();
		}

	}

	outs << "time: " << clock.toc() << endl;
	outs << "done." << endl;
	outs << "skyline items: " << skyline.size() << endl;
	
	logger.log(&outs);

	delete reader;

	outs << "writing to file" << endl;
	logger.log(&outs);

	for(auto item : skyline)
	{
		for(auto vi :item)
		{
			outs << vi << "\t";
		}
		outs << endl;
		logger.log(&outs, true);
	}
	outs << "done." << endl;
	logger.log(&outs);

}

int main(int argc, char** argv)
{
	auto sortComparer = [](const DataPoint& a, const DataPoint& b) {
		return a.minDist < b.minDist;
	};

	auto heapComparer = [](const DataPoint& a, const DataPoint& b) {
		return a.minDist > b.minDist;
	};

	stringstream outs;

	try
	{
		wall_clock wc;

		CommandlineArgs args(argc, argv);

		if (args.ContainsOption("-piso"))
		{
			output_identical_skyilne_object = true;
		}

		if (args.ContainsOption("-echo_command_line")) {
			outs << argv[0] << " " << args.GetArgsLine() << endl;
		}

		if (args.ContainsOption("-txt2fvec"))
		{
			string textfile = args.GetParam("-txt2fvec");
			string delimiter = args.ContainsOption("-delimiter") ? args.GetParam("-delimiter") : " ";
			bool skip_first_col = args.ContainsOption("-skip_first_col");


			DataReader* reader = new DataReader(textfile.c_str());
			FvecWriter* writer = new FvecWriter(textfile + ".bin");

			while(!reader->eof())
			{
				char*line = reader->Readline();
				auto v = DataReader::LineToFloatVector(line, delimiter.c_str());
				if (v.size() > 0) {
					if (skip_first_col)
					{
						v.erase(v.begin());
					}
					writer->Write(v);
				}
			}
			
			delete reader;
			delete writer;

			return 0;
		}

		if (args.ContainsOption("-fvec2txt"))
		{
			string textfile = args.GetParam("-fvec2txt");

			FvecReader* reader = new FvecReader(textfile);
			TextWriter* writer = new TextWriter(textfile + ".txt");

			while (!reader->eof())
			{
				vector<float> data;
				if (reader->ReadNext(data)) {
					if (data.size() > 0)
						writer->WriteLine(data);
				}
			}

			delete reader;
			delete writer;

			return 0;
		}

		if (args.ContainsOption("-mem"))
		{
			useMemoryOnly = true;
		}

		if (args.ContainsOption("-write_heap"))
		{
			includeHeapContent = true;
		}

		if (args.ContainsOption("-use_bnl"))
		{
			useBNL = true;
		}

		if (args.ContainsOption("-create_dataset"))
		{
			if (args.ContainsOption("-d"))
			{
				string output_file = "random_dataset.bin";
				int r1 = 0;
				int r2 = 255;
				

				if (args.ContainsOption("-o"))
				{
					output_file = args.GetParam("-o");
				}

				if (args.ContainsOption("-r1"))
				{
					r1 = atoi(args.GetParam("-r1").c_str());
				}

				if (args.ContainsOption("-r2"))
				{
					r2 = atoi(args.GetParam("-r2").c_str());
				}

				int num_of_samples = atoi(args.GetParam("-create_dataset").c_str());
				int d = atoi(args.GetParam("-d").c_str());

				boost::random::mt19937 rng;
				boost::random::uniform_int_distribution<> random_numbers(r1, r2);

				FvecWriter* writer = new FvecWriter(output_file);

				float* data = new float[d];

				for(int i=0; i< num_of_samples;i++)
				{
					for (int di=0;di < d;di++)
					{
						data[di] = random_numbers(rng);
					}

					vector<float> v(data, data + d);
					writer->Write(v);
				}

				delete writer;
				
			}
			else 
			{

				outs << "param -create_dataset needs also parameter -d to define the dimensions of the vectors." << endl;
				logger.log(&outs);
				return 1;
			}

			return 0;
		}

		if (args.ContainsOption("-just_hashing"))
		{
			justHashing = true;
		}

		//use k dimensions with highest cardinality 
		int k = args.ContainsOption("-k") ? atoi(args.GetParam("-k").c_str()) : -1;

		//output temp path
		auto now = std::chrono::system_clock::now().time_since_epoch().count();
		string temp_path = "output-" + to_string(now);
		boost::filesystem::create_directory(temp_path);

		//hash algorithm
		string hash_algoritm = args.ContainsOption("-h") ? args.GetParam("-h") : "lsh";
		uword datasetDimensions = k == -1 ? atoi(args.GetParam("-d").c_str()) : k;
		uword encodingLength = args.ContainsOption("-e") ? atoi(args.GetParam("-e").c_str()) : 10;
		uword update_interval = args.ContainsOption("-update_rate") ? atoi(args.GetParam("-update_rate").c_str()) : 5;
		bool ignore_zero_distance = !args.ContainsOption("-include_zero_distance");
		uword training_length = args.ContainsOption("-t") ? atoi(args.GetParam("-t").c_str()) : 10000;
		string file = args.ContainsOption("-i") ? args.GetParam("-i") : "c:/!skyline/sift/sift_learn.fvecs";


		map<int, int>* dimension_mapping = nullptr;
		if (k > -1) {
			dimension_mapping = new map<int, int>();

			outs << "PRE-PROCESSING DATASET..." << endl;
			logger.log(&outs);
			auto dataset_cardinality = Common::GetValueCardinality(file);

			outs << endl;
			for (auto i = 0; i < k; i++)
			{
				dimension_mapping->insert(pair<int, int>(i, dataset_cardinality.at(i)[0]));
				outs << "V(" << i << ") -> MapsToDataset(" << dataset_cardinality.at(i)[0] << "), Size:" << dataset_cardinality.at(i).size() - 1 << endl;
			}

			outs << endl << "done." << endl;
			logger.log(&outs);
		}

		if(useBNL)
		{
			bnl_skyline(file, dimension_mapping, ignore_zero_distance);
		}
		else
		{
			wc.tic();
			
			HashingBase* hash = GetHashing(hash_algoritm, file, training_length, datasetDimensions, encodingLength, dimension_mapping);

			auto training_time = wc.toc();
			outs << "Selected hash algorithm : " << hash->GetHashingName() << endl;
			logger.log(&outs);

			Buckets buckets;
			outs << "Hashing dataset..." << endl;
			logger.log(&outs);

			FvecReader r(file);
			int i = 0;

			map<string, FvecWriter*> bucket_files;
			FvecWriter* writer;

			while (!r.eof())
			{
				i++;
				vector<float> data;

				bool read_successfully;
				if (k == -1)
				{
					read_successfully = r.ReadNext(data);
				}
				else
				{
					read_successfully = r.ReadNext(data, dimension_mapping);
				}

				if (read_successfully) {

					auto distance = sum(data);

					//DO NOT include vectors that have 
					//zero value.
					if (distance == 0 && ignore_zero_distance) {
						zero_vectors_count++;
						continue;
					}

					auto h = hash->Hash(arma::conv_to<mat>::from(data).t());
					auto binary_code = Common::CompressRowVector(h.row(0));

					//split input file to bucket files
					if (!useMemoryOnly) {
						auto fi = bucket_files.find(binary_code);

						if (fi == bucket_files.end())
						{
							if (bucket_files.size() > 499)
							{
								int i2 = 0;
								for (auto fi2 = bucket_files.begin(); i2 < 25; ++fi2)
								{
									delete (*fi2).second;
									i2++;
								}

								for (int i3 = 0; i3 < 25; i3++)
									bucket_files.erase(bucket_files.begin());
							}
							bucket_files[binary_code] = new FvecWriter(temp_path + "/" + binary_code, true);
						}
					}

					DataPoint p(i);
					if (useMemoryOnly)
						p.SetData(data);

					p.key = binary_code;
					p.type = 0;
					p.minDist = distance;

					//write vector to file bucket
					if (!useMemoryOnly) {
						bucket_files[binary_code]->Write(reinterpret_cast<void*>(&i), sizeof(int));
						bucket_files[binary_code]->Write(data);
					}
					buckets[binary_code].insert(p);
					
				}
			}
			//print times
			auto hashing_total = wc.toc();
			auto hashing_time = hashing_total - training_time;
			outs << "TR\tHS\tTT" << endl;
			outs << training_time << "\t" << hashing_time << "\t" << hashing_total << endl;

			outs << "hashing complete." << endl;
			logger.log(&outs);


			if (!useMemoryOnly)
			{
				outs << "sorting..." << endl;
				logger.log(&outs);

				for (auto fi = bucket_files.begin(), fend = bucket_files.end(); fi != fend; ++fi)
				{
					delete (*fi).second;
				}
				bucket_files.clear();

				//sort each file by mindist
				//and write them back
				sort_files(buckets, temp_path);
			}


			outs << "done." << endl;
			logger.log(&outs);

			outs << "Preparing heap..." << endl;
			logger.log(&outs);

			logger.log("bucket key           \tmin\tmax\tsize\tmin count", true);

			vector<DataPoint> heap;
			for (auto it = buckets.begin(), end = buckets.end(); it != end; ++it)
			{
				float min = FLT_MAX;
				float max = 0;

				for (auto vi = it->second.begin(), vend = it->second.end(); vi != vend; ++vi) {

					auto s = (*vi).minDist;
					if (s < min)
						min = s;

					if (s > max)
						max = s;
				}

				int min_count = 0;

				for (auto vi = it->second.begin(), vend = it->second.end(); vi != vend; ++vi) {
					if ((*vi).minDist == min)
					{
						min_count++;
					}
				}

				//insert into heap only
				//buckets (intermediate) nodes
				DataPoint heap_node(-1);
				heap_node.minDist = min;
				heap_node.key = it->first;
				heap_node.type = 1;

				heap.push_back(heap_node);

				logger.log(it->first + "\t" + to_string(min) + "\t" + to_string(max) + "\t" + to_string(it->second.size()) + "\t" + to_string(min_count), true);
			}
			outs << "done." << endl;
			logger.log(&outs);

			if (justHashing)
				return 0;



			//create heap
			make_heap(heap.begin(), heap.end(), heapComparer);

			//skyline is sorted by mindist
			Skyline skyline;

			uword expand_count = 0;
			double time_point = wc.toc();
			vector<float> min_projection;

			for (int c = 0; c<datasetDimensions; c++)
			{
				min_projection.push_back(FLT_MAX);
			}

			if (includeHeapContent) {
				outs << "Heap content" << endl;
				logger.log(&outs, true);
			}

			//find skyline main loop
			while (heap.size() > 0)
			{
				auto v = heap.front();

				//move the from element to the end
				pop_heap(heap.begin(), heap.end(), heapComparer);
				heap.pop_back();

				if (includeHeapContent) {
					outs << v.key << "\t" << v.minDist << endl;
					logger.log(&outs, true);
				}

				// v.type
				// 1 == intermediate node 
				// 0 == leaf node
				if (v.type == 1)
				{
					expand_count++;

					if (useMemoryOnly)
					{
						//expand node
						for (auto bi = buckets[v.key].begin(), bend = buckets[v.key].end(); bi != bend; ++bi)
						{
							heap.push_back(*bi);
							push_heap(heap.begin(), heap.end(), heapComparer);
						}
					}
					else
					{
						//read the bucket from file and 
						//add the point to the heap
						FvecReader* bucket_reader = new FvecReader(temp_path + "/" + v.key + ".bin");
						while (!bucket_reader->eof())
						{
							int id = -1;
							if (bucket_reader->Read(&id, sizeof(int)))
							{
								DataPoint p(id);
								vector<float> data;
								if (bucket_reader->ReadNext(data)) {

									p.SetData(data);
									p.key = v.key;

									heap.push_back(p);
									push_heap(heap.begin(), heap.end(), heapComparer);
								}
							}
						}

						delete bucket_reader;
						boost::filesystem::remove(temp_path + "/" + v.key + ".bin");
					}
				}
				else
				{
					//test if skyline point
					bool is_current_point_dominated = false;
					rowvec current_vector = conv_to<rowvec>::from(v.Data());
					rowvec min_skyline_vector = conv_to<rowvec>::from(min_projection);


					//compare with min_skyline_vector
					auto sumMin = sum(current_vector < min_skyline_vector);

					//only if v has some point smaller than min_skyline_vector
					//put it in skyline else (==0) compare v with skyline bucket
					if (sumMin == 0)
					{
#if SKYLINE_BUCKET == 1
						for (auto si = skyline[v.key].begin(), send = skyline[v.key].end(); si != send; ++si)
#else
						for (auto si = skyline.begin(), send = skyline.end(); si != send; ++si)
#endif
						{
							rowvec s_point = conv_to<rowvec>::from((*si).Data());

							auto sumComp = sum(s_point <= current_vector);

							if (output_identical_skyilne_object && sum(s_point == current_vector) == s_point.n_cols)
							{
								is_current_point_dominated = false;
								break;
							}

							//s_point is better or the same 
							//than current_vector. (current_vector is dominated)
							if (sumComp == s_point.n_cols)
							{
								is_current_point_dominated = true;
								break;
							}

							if (sumComp == 0)
							{
								outs << "current point" << endl;
								logger.log(&outs);
								logPoint(v);

								outs << "skyline point" << endl;
								logger.log(&outs);
								logPoint(*si);

								throw ApplicationException("ERROR. skyline point is dominated... there is a logic error. please check!");
							}
						}
					}

					if (!is_current_point_dominated)
					{

#if SKYLINE_BUCKET == 1
						skyline[v.key].insert(v);
#else
						skyline.insert(v);
#endif
						for (int c = 0; c < datasetDimensions; c++)
						{
							if (v.Data()[c] < min_projection[c])
							{
								min_projection[c] = v.Data()[c];
							}
						}
					}

					//print statistics to the user
					if (wc.toc() - time_point >= update_interval)
					{
#if SKYLINE_BUCKET == 1
						int ssize = 0;
						for (auto bi = skyline.begin(), bend = skyline.end(); bi != bend; ++bi)
						{
							auto skyline_bucket = (*bi).second;
							ssize += skyline_bucket.size();
						}
						cout << "[" << wc.toc() << "], exp_count: " << expand_count << ", heap.size():" << heap.size() << ", skyline.size() : " << skyline.size() << ", v.minDist : " << v.minDist << ", total skyline size :" << ssize << endl;
#else
						cout << "[" << wc.toc() << " sec], expand count: " << expand_count << ", heap items: " << heap.size() << ", skyline items: " << skyline.size() << ", current node minDist: " << v.minDist << endl;
#endif
						time_point = wc.toc();
					}
				}
			}

			outs << "Total time : " << wc.toc() << endl;

			//write skyline to file
			log_skyline(skyline);

			outs << "Items read : " << i - 1 << endl;
			outs << "Zero Vectors (skipped): " << zero_vectors_count << endl;
			outs << "Total unique keys : " << buckets.size() << endl;
			
			logger.log(&outs);
		}
	}
	catch (exception e)
	{
		outs << e.what() << endl;
		logger.log(&outs);

		return 1;
	}

	outs << endl << "Completed." << endl;
	logger.log(&outs);
	return 0;
}

