#include "stdafx.h"
#include "SphericalHashing.h"


mat SphericalHashing::randomCenters(mat data, uword encodingBits)
{
	mat centers = mat(encodingBits, data.n_cols).fill(0);

	for (uword i = 0; i < encodingBits; i++) {

		uvec R = Common::RandomPermutation(data.n_rows);
		mat sample = data.rows(R.rows(0, 4));
		sample = arma::sum(sample, 0) / 5.0;
		centers.row(i) = sample;

	}

	return centers;
}

ComputationResult SphericalHashing::computeStatistics(mat data, mat centers)
{
	ComputationResult result;

	uword N = data.n_rows;
	uword D = data.n_cols;
	uword bit = centers.n_rows;

	mat dist = distMat(centers, data);
	mat sort_dist = arma::sort(dist, 0, 1);
	result.radii = sort_dist(span::all, N / 2);
	dist = arma::conv_to<mat>::from(dist <= arma::repmat(result.radii, 1, N));
	dist = dist * 1.0;

	result.O1 = sum(dist, 1);

	result.avg = 0;
	double avg2 = 0;

	result.O2 = dist * dist.t();
	for (uword i = 0; i < bit - 1; i++)	{

		for (uword j = i + 1; j < bit; j++)	{

			result.avg = result.avg + abs(result.O2(i, j) - N / 4.0);
			avg2 = avg2 + result.O2(i, j);

		}

	}

	result.avg = result.avg / (bit * (bit - 1) / 2.0);

	avg2 = avg2 / (bit * (bit - 1) / 2.0);
	result.stddev = 0;
	for (uword i = 0; i < bit - 1; i++)	{

		for (uword j = i + 1; j < bit; j++)	{

			result.stddev = result.stddev + pow((result.O2(i, j) - avg2), 2);

		}
	}

	result.stddev = sqrt(result.stddev / (bit * (bit - 1) / 2.0));
	return result;
}

mat SphericalHashing::distMat(mat p1, mat p2)
{
	mat x1 = arma::repmat(arma::sum(arma::square(p1), 1), 1, p2.n_rows);
	mat x2 = arma::repmat(arma::sum(arma::square(p2), 1), 1, p1.n_rows);

	mat r = p1*p2.t();

	return arma::sqrt(x1 + x2.t() - 2 * r);
}

SphericalHashing::SphericalHashing(mat trainingData, uword encodingBits)
{
	_encodingLength = encodingBits;

	uword N = trainingData.n_rows;
	uword D = trainingData.n_cols;

	_centers = randomCenters(trainingData, _encodingLength);
	ComputationResult res = computeStatistics(trainingData, _centers);
	uword iter = 1;

	while (true) {

		mat forces = mat(_encodingLength, D).fill(0);

		for (uword i = 0; i < _encodingLength - 1; i++) {

			for (uword j = i + 1; j < _encodingLength; j++) {
				auto force = 0.5 * (res.O2(i, j) - N / 4) / (N / 4) * (_centers(i, span::all) - _centers(j, span::all));
				forces(i, span::all) = forces(i, span::all) + force / _encodingLength;
				forces(j, span::all) = forces(j, span::all) - force / _encodingLength;
			}

		}

		_centers = _centers + forces;
		res = computeStatistics(trainingData, _centers);

		if (res.avg <= 0.1 * N / 4 && res.stddev <= 0.15 * N / 4) {
			break;
		}

		if (iter > 100) {
			printf("iter > 100 ... iteration :%u , avg:%f (%f) , stddev: %f (%f) \n", iter, res.avg, 0.1 * N / 4, res.stddev, 0.15 * N / 4);
		}

		iter++;
	}
	_radii = res.radii;
}

SphericalHashing::~SphericalHashing()
{
}

mat SphericalHashing::Hash(mat data)
{
	mat dData = distMat(data, _centers);
	mat th = repmat(_radii.t(), dData.n_rows, 1);
	umat bData = umat(dData.n_rows, dData.n_cols).fill(0);

	uvec x = find( dData <= th);

	for (uword i = 0; i < x.n_rows; i++) {
	
		bData(x(i)) = 1;

	}

	return arma::conv_to<mat>::from(bData);
}



