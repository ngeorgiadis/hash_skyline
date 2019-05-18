#include "stdafx.h"
#include "CommandlineArgs.h"


CommandlineArgs::CommandlineArgs(int argc, char** argv)
{

	if (argc%2 == 0 && argc > 1)
	{
		throw ApplicationException("args are not correct");
	}

	for (int i = 1; i<argc-1; i+=2)
	{
		_args_map.insert(pair<string, string>(argv[i], argv[i + 1]));
	}
}

CommandlineArgs::~CommandlineArgs()
{
}
