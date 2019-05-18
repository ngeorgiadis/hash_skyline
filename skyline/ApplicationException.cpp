#include "stdafx.h"
#include "ApplicationException.h"


ApplicationException::ApplicationException()
{
}

ApplicationException::ApplicationException(char* msg)
	: exception(msg)
{
	_msg = msg;
}

ApplicationException::ApplicationException(string msg)
	: exception(msg.c_str())
{
	_msg = const_cast<char*>(msg.c_str());
}

ApplicationException::~ApplicationException()
{
}
