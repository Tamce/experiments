#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <sstream>
#include "Socket.h"

using std::cout;
using std::endl;
using std::cin;
using std::thread;
using std::stringstream;
using std::string;
using tmc::ClientSocket;
using tmc::Socket;

#pragma comment (lib, "WS2_32.lib")

void showHelp();
void serviceProvider();
void resolveRequest(stringstream &str)
{
    cout << "\nClient says:\n" << str.str() << endl;
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
	cin.get();
	serviceThread.detach();
	return 0;
}

void serviceProvider()
{
	try
	{
		Socket serverSocket;
		char buff;
		stringstream ss;
		cout << "\nCreating a service socket listening on port 21...";
		serverSocket.openListen("21");
		cout << "\nListening port 21...";
		while (true)
		{
			ClientSocket client = serverSocket.accept();
			cout << "\nClient connected! Fetching data...";
			ss.str("");
			while (client.get(buff))
			{
				if (buff == '\n')
					break;
				ss << buff;
			}
			client.send("You said: ");
			client.send(ss.str().c_str());
			client.shutdownClose();
			resolveRequest(ss);
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
Tamce File Transmit Protocal(TSFTP)\n\
         Server Side Program\n\
\n\
Usage:\n\
\n\
tftpd\
";

}