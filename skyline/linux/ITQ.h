#pragma once
class ITQ : public HashingBase
{

	uword _iterations = 50;
	uword _encodingLength;
	mat _data;
	mat _pc;
	mat _r;
	

public:
	
	ITQ(mat trainData, uword encodingLength);
	~ITQ();

	mat Hash(mat data) override;
	void Print_R() const;
	
	mat PC() const;
	static mat PCA(mat x, uword bit);

	string GetHashingName() override
	{
		return "Iterative Quantization Hashing (ITQ)";
	}

	uword GetEncodingLength() override {
		return _encodingLength;
	}
};

