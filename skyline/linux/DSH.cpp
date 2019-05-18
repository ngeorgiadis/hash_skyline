#include "stdafx.h"
#include "DSH.h"


mat DSH::euDist2(mat a, mat b, bool useSqrt)
{
	mat aa = arma::sum(a % a, 1);
	mat bb = arma::sum(b % b, 1);
	mat ab = a * b.t();
	
	mat d = (arma::repmat(aa,1,ab.n_cols) + arma::repmat(bb.t(),ab.n_rows,1)) - 2*ab;

	for (uword i = 0; i < d.n_rows; i++) {
		for (uword j = 0; j < d.n_cols; j++) {

			if (d(i, j) < 0) {
				d(i, j) = 0;
			}
		}
	}

	if (useSqrt) {
		d = arma::sqrt(d);
	}
	return d;
}

DSH::DSH(mat trainingData, uword encodingBits)
{

	_encodingLength = encodingBits;

	double alpha = 1.5;
	uword r = 3;
	uword iter = 3;
	
	uword cluster = (uword)round(_encodingLength * alpha);
	
	KMeans kmeans(cluster, trainingData);
	mat U = kmeans.Cluster2();

	
	//if U has NaN then atleast one cluster
	//has been eliminated. 
	//Maybe use more training data
	uvec dump = kmeans.GetClusterIndex();
	
	uword nSamples = trainingData.n_rows;
	uword nFeatures = trainingData.n_cols;
	
	_model_U = mat(_encodingLength, nFeatures).fill(0);
	_model_intercept = vec(_encodingLength, 1).fill(0);

	vec clusize = vec(U.n_rows, 1).fill(0);

	for (uword i = 0; i < U.n_rows; i++) {

		uvec f = arma::find(dump == i);
		clusize.row(i) = f.n_rows;
		//cout << "i:" << i << ", size:"<< f.size() << endl;
	}
	
	clusize = clusize / sum(clusize);
	mat du = euDist2(U, U, false);
	
	for (uword i = 0; i < du.n_rows; i++) {
		du(i, i) = datum::inf;
	}
	
	//du.print();
	mat dr = mat(du.n_rows, r);
	
	for (uword i = 0; i < du.n_rows; i++) {

		rowvec tmp = du.row(i);
		//tmp.print();

		rowvec tmpsort = arma::sort(tmp);
		dr.row(i) = tmpsort.cols(1, r);

	} 
	
	dr = arma::unique(dr);

	vec bitsize = vec(dr.n_rows);
	
	//U.print();
	for (uword i = 0; i < bitsize.n_rows; i++) {

		uvec a = arma::find(du == dr(i), 1, "first");

		uword col = a(0) % du.n_cols;
		uword row = (uword)floor(a(0) / (double) du.n_cols);

		rowvec tmp1 =  (U.row(row) + U.row(col)) / 2.0;
		vec tmp2 = (U.row(row) - U.row(col)).t();
		mat tmp3 = repmat(tmp1, U.n_rows, 1);
		auto DD = U * tmp2;
		auto th = tmp3 * tmp2;
		auto pnum = find(DD > th);
		auto tmpnum = clusize(pnum);
		auto num1 = sum(tmpnum);
		auto num2 = 1 - num1;
	
		bitsize(i) = min(num1, num2) / max(num1, num2);
	}

	vec Dsorts = sort(bitsize, "descend");

	for (uword i = 0; i < _encodingLength; i++) {

		uvec ids = arma::find(bitsize == Dsorts(i), 1, "first");

		uvec a = find(du == dr(ids(0)), 1, "first");
		uword col = a(0) % du.n_cols;
		uword row = (uword)floor(a(0) / (double)du.n_cols);

		du(row, col) = datum::inf;
		du(col, row) = datum::inf;

		bitsize(ids(0)) = datum::inf;

		_model_U.row(i) = U.row(row) - U.row(col);
		_model_intercept.row(i) = ((U.row(row) + U.row(col)) / 2.0)* _model_U.row(i).t();

	}

	mat res = repmat(_model_intercept.t(), nSamples, 1);
	mat Ym = trainingData * _model_U.t();
	umat B = (Ym > res);
	//B.print("Matrix B:");
}

DSH::~DSH()
{
}

mat DSH::Hash(mat data) 
{
	mat res = arma::repmat(_model_intercept.t(), data.n_rows, 1);
	mat U = data * _model_U.t();
	umat b = (U > res);
	return arma::conv_to<mat>::from(b);
}
