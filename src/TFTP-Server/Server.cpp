#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <thread>
#include <sstream>
#include <exception>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include <memory>
#include <fstream>
#include "../Socket.h"

using std::cout;
using std::endl;
using std::cin;
using std::vector;
using std::srand;
using std::rand;
using std::thread;
using std::stringstream;
using std::ofstream;
using std::ifstream;
using std::string;
using std::unique_ptr;
using std::exception;
using tmc::ClientSocket;
using tmc::Socket;

#pragma comment (lib, "WS2_32.lib")

inline void log(const char *message, const char *message2 = nullptr)
{
	if (nullptr == message2)
		message2 = "";
	cout << message << message2;
}

void showHelp();
void serviceProvider();
unique_ptr<thread> acceptConnection(ClientSocket &);

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
	cin.get();
	serviceThread.detach();
	return 0;
}

// This function is the place where program really resolve the clients' request.
void working(string port)
{
	Socket socket;
	char buffer[512];
	string str;
	// Only accept one connection
	log("\nClient served on port ", port.c_str());
	socket.openListen(port.c_str(), 1);
	ClientSocket client = socket.accept();
	// Resolve request
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		if (0 == client.gets(buffer, 3))
		{
			cout << "\nLost client connection on port " << port << ".";
			return;
		}
		str = buffer;
		if (str == "HEY")
		{
			client.sendWithZero("Hi, This is a simple TFTP(Tamce File Transfer Protocol) server build up by tamce.\n");
		} else if (str == "SND")
		{
			// remove space
			client.gets(buffer, 1);
			// get file name
			client.gets(buffer, 32);
			cout << "\nReciving file " << buffer << " from client: " << port;
			ofstream file(buffer, std::ios::out | std::ios::binary);
			// get size of file
			client.gets(buffer, 4);
			cout << "\nFile length: " << *(int *)buffer;
			for (int n = *(int *)buffer; n > 0;)
			{
				int count = client.gets(buffer, min(512, n));
				cout << "\n" << count << " bytes recieved from client " << port;
				n -= count;
				file.write(buffer, count);
			}
			file.close();
			cout << "\nAll bytes recieved from client " << port;
			client.sendWithZero("File successfuly saved on server.");
		} else if (str == "LST")
		{
			system("set /p = <nul > ____dir.temp");
			system("attrib +h ____dir.temp 2>nul");
			system("dir /a:-d-h-s /b >> ____dir.temp");
			ifstream dir("____dir.temp");
			while (getline(dir, str))
			{
				client.send(str.c_str());
				client.send("\n", 1);
			}
			dir.clear();
			dir.close();
			client.send("\0", 1);
			system("attrib -h ____dir.temp 2>nul");
			system("del /f /q ____dir.temp 2>nul");
		} else if (str == "GET")
		{
			// remove space
			client.gets(buffer, 1);
			// get file name
			client.gets(buffer, 32);
			cout << "\nClient " << port << " requesting file " << buffer;
			ifstream file(buffer, std::ios::in | std::ios::binary);
			file.seekg(0, std::ios::end);
			int length = file.tellg();
			cout << "\nSending file " << buffer << " to client " << port << " with length " << length;
			memcpy(buffer, &length, sizeof(length));
			// send file size
			client.send(buffer, sizeof(length));
			file.seekg(0, std::ios::beg);

			while (file)
			{
				file.read(buffer, 512);
				cout << "\nSent " << client.send(buffer, file.gcount()) << " bytes to client " << port << ".";
			}
			cout << "\nAll bytes sent to client " << port << ".";
			file.clear();
			file.close();
		} else if (str == "BYE")
		{
			client.sendWithZero("Bye!");
			client.shutdownClose();
			// go out of the loop
			break;
		}
	}
	cout << "\nClosing connecton on port " << port << ".";
	return;
}

// Accept a connection, send a random port to client.
// Tell the client to connect to that port.
unique_ptr<thread> acceptConnection(ClientSocket &client)
{
	srand(time(nullptr));
	unique_ptr<thread> accepted;
	int port = rand() % 1024 + 50000;
	char buff[8] = {};
	_itoa(port, buff, 10);
	accepted = unique_ptr<thread>(new thread(working, string(buff)));
	client.send(buff);
	client.shutdownClose();
	return accepted;
}

void serviceProvider()
{
	try
	{
		Socket serverSocket;
		vector<unique_ptr<thread>> workingThread;
		log("\nBooting program...");
		log("\nCreating a service socket listening on port 21...");
		serverSocket.openListen("21");
		log("\nMain service listening on port 21...");
		log("\nPress Ctrl+C to exit the program.");
		log("\n============= Ready =============\n");
		while (true)
		{
			ClientSocket client = serverSocket.accept();
			workingThread.push_back(acceptConnection(client));
			// TODO clean stoped working thread
		}
	} catch (exception e)
	{
		std::cerr << "An error occurred!\n" << e.what();
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
tftpd\n\
";

}
