#pragma once
#include <sstream>
#include <exception>
namespace tmc
{
    class Exception : public std::exception
    {
    public:
		std::stringstream buff;
        Exception(const char *m, int code = 0)
        {
			buff.str(m);
			buff << "\tError Code: " << code;
        }
		virtual const char *what()
		{
			return buff.str().c_str();
		}
    };
}
