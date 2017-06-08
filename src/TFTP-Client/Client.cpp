#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <utility>
#include <thread>
#include <mutex>
#include <queue>
#include "../Socket.h"

using std::cout;
using std::cin;
using std::cerr;
using std::stringstream;
using std::istream;
using std::string;
using std::queue;
using std::thread;
using std::mutex;
using tmc::Socket;
using tmc::ClientSocket;

#pragma comment (lib, "WS2_32.lib")

class Task
{
public:
	enum Type
	{
		CONNECT, SENDSTREAM, SHUTDOWN
	};
	Type type;
	string args;
	Task(Type _type, string &_args = string()):type(_type), args(_args){}
	Task(const Task &task):type(task.type), args(task.args){}
};

void resolveTasks();
void agent();

queue<Task> tasks;
mutex lock;
mutex outputLock;

int main()
{
	
	thread messager(resolveTasks);
	thread master(agent);
	master.join();
	messager.detach();
	
	/*
	Socket socket;
	ClientSocket c;
	c = std::move<ClientSocket>(socket.connect("127.0.0.1", "21"));
	c.send("Hello");
	c.shutdownClose();
	*/
	return 0;
}

void resolveTasks()
{
	bool connected = false;
	Socket socket;
	ClientSocket clientSocket;
	stringstream sstr;
	char buff[100];
	while (true)
	{
		// todo Check if there is data coming, if so, print it to the screen
		// if tasks is empty, then wait until it is not empty
		if (tasks.empty())
		{
			continue;
		}
		outputLock.lock();
		cerr << "\nPopping a task...";
		lock.lock();
		Task task = tasks.front();
		tasks.pop();
		lock.unlock();

		switch (task.type)
		{
			case Task::CONNECT:
				if (connected)
				{
					cerr << "\nThere is an active connection, please shutdown first!";
					break;
				}
				sstr.str(task.args);
				sstr >> buff >> &buff[50];
				sstr.clear();
				cerr << "\nConnecting to " << buff << ":" << &buff[50];
				clientSocket = socket.connect(buff, &buff[50]);
				connected = true;
				break;
			case Task::SENDSTREAM:
				if (!connected)
				{
					cerr << "\nThere is no active connection, please connect first!";
					break;
				}
				cerr << "\nSending " << task.args.length() << " bytes data.";
				clientSocket.send(task.args.c_str());
				cerr << "\nAll data sent.";
				break;
			case Task::SHUTDOWN:
				if (!connected)
				{
					cerr << "\nThere is no active connection, please connect first!";
					break;
				}
				cerr << "\nShutting down connected socket.";
				clientSocket.shutdownClose();
				connected = false;
				break;
			default:
				cerr << "\nUnknown task type: " << task.type;
				break;
		}
		outputLock.unlock();
	}
}

void agent()
{
	string command, arg;
	stringstream sstr;
	while (true)
	{
		outputLock.lock();
		cout << "\n\n> ";
		outputLock.unlock();
		cin.clear();
		cin.sync();
		cin >> command;
		if (command == "connect")
		{
			arg = "";
			sstr.str("");
			cin >> arg;
			sstr << arg;
			cin >> arg;
			sstr << " " << arg;
			lock.lock();
			tasks.push(Task(Task::CONNECT, sstr.str()));
			lock.unlock();
		} else if (command == "send")
		{
			char c;
			cin >> c;
			cin.unget();
			std::getline(cin, arg);
			lock.lock();
			tasks.push(Task(Task::SENDSTREAM, arg));
			lock.unlock();
		} else if (command == "shutdown")
		{
			lock.lock();
			tasks.push(Task(Task::SHUTDOWN));
			lock.unlock();
		} else if (command == "quit")
			break;
		else
		{
			cerr << "Unknown command: " << command << "\n";
		}
	}
}