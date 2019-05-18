#pragma once
class DSH : public HashingBase
{
	mat _model_U;
	vec _model_intercept;
	uword _encodingLength;
	mat euDist2(mat a, mat b, bool useSqrt);

public:

	DSH(mat trainingData, uword encodingBits);
	~DSH();

	mat Hash(mat data) override;
	string GetHashingName() override
	{
		return "Density Sensitive Hashing (DSH)";
	}

	uword GetEncodingLength() override {
		return _encodingLength;
	}
};

