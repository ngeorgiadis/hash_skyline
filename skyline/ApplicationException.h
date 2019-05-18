#pragma once
class ApplicationException : public runtime_error
{
	char* _msg;
public:
	ApplicationException();
	ApplicationException(char* msg);
	ApplicationException(string msg);
	~ApplicationException();
};

