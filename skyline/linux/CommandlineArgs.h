#pragma once
class CommandlineArgs
{

	map<string, string> _args_map;
public:
	CommandlineArgs(int argc, char** argv);
	~CommandlineArgs();


	string GetParam(string option)
	{
		auto i = _args_map.find(option);

		if (i != _args_map.end())
			return i->second;

		throw ApplicationException("Parameter not found : " + option);
	}

	bool ContainsOption(const char* str)
	{
		auto i = _args_map.find(str);
		if (i == _args_map.end())
			return false;

		return true;
	}

	string GetArgsLine()
	{
		stringstream ss;
		for(auto a : _args_map)
		{
			ss << a.first << " " << a.second << " ";
		}
		return ss.str();
	}
};

