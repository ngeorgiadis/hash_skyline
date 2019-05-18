#pragma once
class Logger
{
	ofstream _logfile;
public:
	Logger();
	~Logger();

	void log(stringstream* msg, bool fileonly = false)
	{
		if (!fileonly)
			cout << msg->str();

		_logfile << msg->str();

		msg->str(std::string());
	}

	void log(string msg, bool fileonly = false)
	{
		if (!fileonly)
			cout << msg << endl;

		_logfile << msg << endl;
	}
	
};

