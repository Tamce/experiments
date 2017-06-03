#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <sstream>
#include "Socket.h"

using std::cout;
using std::thread;
using std::stringstream;
using std::string;
using tmc::ClientSocket;
using tmc::Socket;

#pragma comment (lib, "WS2_32.lib")

void showHelp();
void serviceProvider();
void resolveRequest(string str)
{
    cout << "\nClient says:\n" << str;
}

inline void parseCommand(int &argc, char **argv)
{
    if (argc > 1)
    {
        showHelp();
        exit(0);
    }
}

int main(int argc, char **argv)
{
	thread serviceThread;

	parseCommand(argc, argv);
	serviceThread = thread(serviceProvider);
	// serviceProvider();
	while (true);
}

void serviceProvider()
{
	try
	{
		Socket s;
		char buff;
		stringstream ss;
		cout << "\nSetting up socket and bind port 21...";
		s.init("21");
		cout << "\nTry to listen on port 21...";
		s.listen();
		cout << "\nListening port 21...";
		while (true)
		{
			ClientSocket client = s.accept();
			cout << "\nClient connected! Fetching data...";
			ss.str("");
			while (client.get(buff))
			{
				ss << buff;
			}
			resolveRequest(ss.str());
		}
	} catch (tmc::Exception e)
	{
		std::cerr << e.message << e.code << std::flush;
	}
}

void showHelp()
{
    cout <<
"\n\
Tamce Simple File Transmit Protocal(TSFTP)\n\
         Server Side Program\n\
\n\
Usage:\n\
\n\
tsftpd\
";

}
