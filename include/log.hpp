#pragma once

#include <sstream>

class log
{
public:
	static void error(const std::string &error)
	{
		instance().errorImpl(error);
		std::ostringstream oss;
	    for (const auto& err : instance().errors)
	    {
	        oss << err << "\n";
	    }
	    instance().errorsStr = oss.str();
	}
	static std::string* getString()
	{
	    std::ostringstream oss;
	    for (const auto& err : instance().errors)
	    {
	        oss << err << "\n";
	    }
	    instance().errorsStr = oss.str();
	    return &instance().errorsStr;
	}

	static void clear()
	{
		instance().errors.clear();
		std::ostringstream oss;
	    for (const auto& err : instance().errors)
	    {
	        oss << err << "\n";
	    }
	    instance().errorsStr = oss.str();
	}
private:
	log() = default;
	~log() = default;
	log(const log&) = delete;
	log& operator=(const log&) = delete;

	static log& instance()
	{
		static log instance;
		return instance;
	}

	void errorImpl(const std::string &error)
	{
		errors.push_back(error);
	}

	std::vector<std::string> errors;
	std::string errorsStr;
};