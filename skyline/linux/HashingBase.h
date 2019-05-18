#pragma once
class HashingBase
{
public:
	virtual ~HashingBase()
	{
	}

	virtual mat Hash(mat data)
	{
		throw ApplicationException("No hashing defined");
	};

	virtual string GetHashingName() = 0;
	
	virtual uword GetEncodingLength() = 0;
	
};

