#pragma once
struct ComputationResult {
	mat O1;
	mat O2;
	mat radii;
	double avg;
	double stddev;
};


class SphericalHashing : public HashingBase
{
	mat randomCenters(mat data,uword encodingBits);
	ComputationResult computeStatistics(mat data, mat centers);
	mat distMat(mat p1, mat p2);
	mat _centers;
	mat _radii;

	uword _encodingLength;
public:
	
	SphericalHashing(mat data, uword encodingBits);
	~SphericalHashing();
	mat Hash(mat data) override;
	string GetHashingName() override
	{
		return "Spherical Hashing (Sph)";
	}

	uword GetEncodingLength() override {
		return _encodingLength;
	}
	
};

