#include "stdafx.h"
#include "Logger.h"


Logger::Logger()
{
	auto now = std::chrono::system_clock::now().time_since_epoch().count();
	stringstream filename;

	filename << "log-" << now << ".txt";
	_logfile = ofstream(filename.str(), ios::out);
}


Logger::~Logger()
{
	_logfile.close();
}
