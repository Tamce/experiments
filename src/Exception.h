#pragma once
#include <cstring>
#include <exception>
namespace tmc
{
    class Exception : public std::exception
    {
    public:
        char *const message;
        int code;
        Exception(const char *m, int _code = 0): code(_code), message(new char[strlen(m) + 1])
        {
            std::strcpy(message, m);
        }
        ~Exception()
        {
            delete message;
        }
		const char *what()
		{
			return message;
		}
    };
}
