#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
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

int main()
{
	thread messager(resolveTasks);
	thread master(agent);
	master.join();
	messager.detach();
	/*
	Socket socket;
	ClientSocket c;
	c = socket.connect("127.0.0.1", "21");
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
		// todo 如果有数据传输则读取
		// 如果队列非空，获得锁后处理队列中第一个任务
		if (tasks.empty())
		{
			continue;
		}
		cerr << "\nPopping a task...";
		lock.lock();
		Task task = tasks.front();
		tasks.pop();
		lock.unlock();

		switch (task.type)
		{
			case Task::CONNECT:
				if (connected)
					continue;
				sstr.str(task.args);
				sstr >> buff >> &buff[50];
				cerr << "\nConnecting to " << buff << ":" << &buff[50] << "\n";
				clientSocket = ClientSocket(socket.connect(buff, &buff[50], true));
				connected = true;
				break;
			case Task::SENDSTREAM:
				if (!connected)
					continue;
				cerr << "\nSending" << task.args << "\n";
				clientSocket.send(task.args.c_str());
				cerr << "\nAll data sent.\n";
				break;
			case Task::SHUTDOWN:
				if (!connected)
					continue;
				cerr << "\nShutting down connected socket.\n";
				clientSocket.shutdownClose();
				break;
			default:
				break;
		}
	}
}

void agent()
{
	string command, arg;
	stringstream sstr;
	while (true)
	{
		cout << "> ";
		sstr.str("");
		cin >> command;
		if (command == "connect")
		{
			lock.lock();
			cin >> arg;
			sstr << arg;
			cin >> arg;
			sstr << " " << arg;
			tasks.push(Task(Task::CONNECT, sstr.str()));
			lock.unlock();
		} else if (command == "send")
		{
			lock.lock();
			std::getline(cin, arg);
			tasks.push(Task(Task::SENDSTREAM, arg));
			lock.unlock();
		} else if (command == "shutdown")
		{
			lock.lock();
			tasks.push(Task(Task::SHUTDOWN));
			lock.unlock();
		} else if (command == "quit")
			break;
	}
}