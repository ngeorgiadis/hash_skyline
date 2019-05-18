#include "stdafx.h"
#include "ApplicationException.h"


ApplicationException::ApplicationException()
	:runtime_error("An error occured")
{
}

ApplicationException::ApplicationException(char* msg)
	: runtime_error(msg)
{
	_msg = msg;
}

ApplicationException::ApplicationException(string msg)
	: runtime_error(msg.c_str())
{
	_msg = const_cast<char*>(msg.c_str());
}

ApplicationException::~ApplicationException()
{
}
