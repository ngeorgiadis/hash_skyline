#include "stdafx.h"
#include "KMeans.h"


KMeans::KMeans(uword k, mat data)
{
	_k = k;
	_data = data;
	_kmeans_options.MaxIterations = 15;
	_cluster_index = uvec(data.n_rows);
}

KMeans::~KMeans()
{
}

mat KMeans::Cluster_OLD() {

	
	mat c(_k, _data.n_cols);
	mat temp(_k, _data.n_cols);
	temp.fill(0);

	c.randu();

	for (int k = 0; k < _kmeans_options.MaxIterations; ++k) {

		vector<vector<uword>> indices = vector<vector<uword>>(_k);

		//assign cluster centroids
		for (uword i = 0; i < _data.n_rows; ++i) {

			vec data = _data.row(i).t();

			double min = std::numeric_limits<double>::max();
			uword c_index = 0;

			for (uword center_i = 0; center_i < _k; center_i++) {
				vec center = c.row(center_i).t();
				double d = distance(data, center);
				if ( d < min) {
					//assign class to data row i
					//closest to center center_i : 0..k
					c_index = center_i;
					min = d;
				}
			}

			_cluster_index[i] = c_index;
			indices[c_index].push_back(i);
		}
		
		//set new cluster centers
		for (uword k = 0; k < _k; k++) {
			uvec v = uvec(indices[k]);
			c.row(k) = getMean(_data, v).t();
		}
	}
	return c;
}

mat KMeans::Cluster2()
{
	bool status = kmeans(_means, _data.t(), _k, random_subset, 10, false);
	if (status == false)
	{
		throw ApplicationException("KMeans failed.");
	}
	return _means.t();
}

uvec KMeans::GetClusterIndex()
{
	try 
	{
		for (int i = 0; i < _data.n_rows; i++)
		{
			_cluster_index.row(i) = getLabel(_means, _data.row(i).t());
		}
		return _cluster_index;
	}
	catch(exception e)
	{
		cout << e.what() << endl;
		throw;
	}
}

double KMeans::distance(vec a, vec b) {

	if (a.n_rows != b.n_rows) {
		throw new ApplicationException("vectors are not the same size.");
	}

	double s = 0;
	for (uword i = 0; i < a.n_rows; ++i) {
		s += pow((double)(a(i) - b(i)), 2);
	}

	return sqrt(s);
}
double KMeans::distance2(vec a, vec b) {

	double norm_a = arma::accu(arma::square(a));
	double norm_b = arma::accu(arma::square(b));
	double dot_product = dot(a, b);
	return (dot_product) / (norm_a * norm_b);
}
vec KMeans::getMean(mat a, uvec indices) {

	vec s = vec(a.n_cols);
	s.fill(0);
	
	for (uword i = 0; i < indices.n_rows; i++) {
		uword row = indices(i);
		s += a.row(row).t();
	}
	return s / indices.n_rows;
}


double KMeans::getDistance(colvec a, colvec b) {

	colvec temp = a - b;
	return norm(temp, 2);

}

double KMeans::getLabel(mat centroids, colvec pt) {

	try {

		double minDistance = 0;
		int minLabel = 0;

		for (int i = 0; i < centroids.n_cols; i++) {

			double tempDistance = getDistance(centroids.col(i), pt);

			if (i == 0 || tempDistance < minDistance) {
				minDistance = tempDistance;
				minLabel = i;
			}
		}

		return minLabel;

	}catch(exception e)
	{
		cout << e.what() << endl;
		throw;
	}

	
}