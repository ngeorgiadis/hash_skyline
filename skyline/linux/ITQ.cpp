#include "stdafx.h"
#include "ITQ.h"


ITQ::ITQ(mat trainData, uword encodingLength)
{

	_data = trainData;
	_encodingLength = encodingLength;

	_pc = ITQ::PCA(trainData, encodingLength);
	
	mat R = randn(encodingLength, encodingLength);

	mat U11;
	vec S2;
	mat V2;
	arma::svd_econ(U11, S2, V2, R, "both", "std");

	R = U11.cols(0, encodingLength-1);
	
	for (uword i = 0; i < _iterations; i++) {
		
		mat Z = _pc * R;
		mat UX(Z.n_rows, Z.n_cols);
		UX.fill(-1);
	
		for (uword r = 0; r < Z.n_rows; r++) {
			for (uword c = 0; c < Z.n_cols; c++) {

				if (Z(r, c) >= 0) {
					UX(r, c) = 1;
				}

			}
		}
		
		mat C = UX.t() * _pc;
		mat UB;
		vec sigma;
		mat UA;

		arma::svd_econ(UB, sigma, UA, C, "both", "std");
		R = UA * UB.t();
	}

	_r = R;
}

ITQ::~ITQ()
{
}

mat ITQ::Hash(mat data)
{
	return (data * _pc) * _r;
}

void ITQ::Print_R() const
{
	_r.print("Rotation array");
}

mat ITQ::PC() const
{
	return _pc;
}

mat ITQ::PCA(mat data, uword k)
{
	if (data.n_cols < k) {
		char* buffer = new char[255];
		sprintf(buffer, "parameter K must be less than number of features (data.n_cols : %i, K: %i)", data.n_cols, k);
		throw ApplicationException(buffer);
	}

	uword m = data.n_rows;
	mat sigma = (1 / double(m)) * data.t() * data;

	mat U;
	vec s;
	mat V;

	if (!arma::svd_econ(U, s, V, sigma, "both" ,"std")) {
		throw ApplicationException("SVD failed.");
	}

	return U.cols(1, k);
}
