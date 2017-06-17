#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include "../Socket.h"

using std::cout;
using std::cin;
using std::cerr;
using std::string;
using std::ifstream;
using std::ofstream;
using tmc::Socket;
using tmc::ClientSocket;

#pragma comment (lib, "WS2_32.lib")

void showHelp();
string fetchString(ClientSocket &server)
{
	char c = 'x';
	string str = "";
	while (c != '\0')
	{
		server.get(c);
		str.push_back(c);
	}
	return str;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		showHelp();
		return 0;
	}

	char buff[512] = {};
	Socket socket;
	cout << "Connecting " << argv[1] << ":" << argv[2];
	ClientSocket server = socket.connect(argv[1], argv[2]);
	cout << "\nConnection established!";
	if (0 == server.gets(buff, 8))
	{
		cerr << "\n> Connection closed by server accidently, cannot resolve resonse with length 0!";
		return 1;
	}
	cout << "\n> Server response: " << buff;
	server.shutdownClose();
	cout << "\n> Connecting " << argv[1] << ":" << buff;
	server = std::move<ClientSocket>(socket.connect(argv[1], buff));
	cout << "\n> Connection established! Waiting for commands.";
	while (true)
	{
		static string str, command;
		command = str = "";
		cin.clear();
		cin.sync();
		cout << "\n$ ";
		cin >> command;
		if (command == "LST")
		{
			server.send("LST");
			cout << "\n" << fetchString(server);
		} else if (command == "HEY")
		{
			server.send("HEY");
			cout << "\n" << fetchString(server);
		} else if (command == "BYE")
		{
			server.send("BYE");
			while (server.get(buff[0]))
			{
				cout << buff[0];
			}
			cout << "\n> Lost connection.";
			// exit loop
			break;
		} else if (command == "EXT")
		{
			server.shutdownClose();
			cout << "\n> Closed connection.";
			break;
		} else if (command == "GET")
		{
			// input filename
			cin >> str;
			ofstream file(str, std::ios::out | std::ios::binary);
			memset(buff, 0, 32);
			memcpy(buff, str.c_str(), min(str.length(), 32));
			// send command
			server.send("GET ");
			// send filename
			server.send(buff, 32);
			// get size of file
			server.gets(buff, 4);
			cout << "\n> File length: " << *(int *)buff;
			for (int n = *(int *)buff; n > 0;)
			{
				int count = server.gets(buff, min(512, n));
				cout << "\n> " << count << " bytes recieved.";
				n -= count;
				file.write(buff, count);
			}
			cout << "\n> All bytes recieved.";
			file.close();
			cout << "\n> File saved to " << str;
		} else if (command == "SND")
		{
			// input filename
			cin >> str;
			ifstream file(str, std::ios::in | std::ios::binary);
			memset(buff, 0, 32);
			memcpy(buff, str.c_str(), min(str.length(), 32));
			// send command
			server.send("SND ");
			// send filename
			server.send(buff, 32);

			// send file size
			file.seekg(0, std::ios::end);
			int length = file.tellg();
			cout << "\n> Sending file with length: " << length;
			memcpy(buff, &length, sizeof(length));
			server.send(buff, sizeof(length));
			file.seekg(0, std::ios::beg);

			while (file)
			{
				file.read(buff, 512);
				cout << "\n> Sent " << server.send(buff, file.gcount()) << " bytes.";
			}
			cout << "\n> All bytes sent.";
			file.clear();
			file.close();
			cout << "\n" << fetchString(server);
		} else
		{
			cout << "\n> Invalid command! Please re-input a command.";
		}
	}
	cout << "\n> Press anykey to exit.";
	system("pause >nul");
	return 0;
}

void showHelp()
{
	cout <<
		"\n\
Tamce File Transmit Protocal(TSFTP)\n\
         Client Side Program\n\
\n\
Usage:\n\
\n\
tftp addr port\n\
\n\
addr   \tAddress to connect to\n\
port   \tTarget port\
";
}