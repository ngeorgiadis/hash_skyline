#pragma once
class LSH : public HashingBase
{
	mat w;
	uword _encoding_length;
public:
	LSH(uword dimensions, uword encodingBits);
	~LSH();

	mat Hash(mat data) override;
	string GetHashingName() override {
		return "Locality Sensitive Hashing (LSH)";
	}

	uword GetEncodingLength() override {
		return _encoding_length;
	}
};

