#pragma once
class ApplicationException : public exception
{
	char* _msg;
public:
	ApplicationException();
	ApplicationException(char* msg);
	ApplicationException(string msg);
	~ApplicationException();
};

