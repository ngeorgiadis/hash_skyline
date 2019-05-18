#include "stdafx.h"
#include "LSH.h"

LSH::LSH(uword dimensions, uword encodingBits)
{
	// create the random projection matrix
	w = randn<mat>(dimensions, encodingBits);
	_encoding_length = encodingBits;
}

LSH::~LSH()
{
}

mat LSH::Hash(mat data)
{
	umat u = (data * w) > 0;
	return arma::conv_to<mat>::from(u);
}

