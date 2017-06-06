#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <queue>
#include "Socket.h"

using std::cout;
using std::cin;
using std::cerr;
using std::stringstream;
using std::string;
using std::queue;
using std::thread;
using std::mutex;
using tmc::Socket;
using tmc::ClientSocket;

class Task;
void resolveTasks(queue<Task> &tasks);

mutex lock;

int main()
{
	queue<Task> tasks;
	thread messager(resolveTasks, tasks);
	
	messager.detach();
	return 0;
}

class Task
{
public:
	enum Type {CONNECT, SENDSTREAM, SHUTDOWN};
	Type type;
	stringstream args;
	Task(Type _type, string _args = ""):type(type), args(_args) {}
	Task(Task &task):type(task.type), args(task.args) {}
};

void resolveTasks(queue<Task> &tasks)
{
	while (true)
	{
		// 如果队列非空，获得锁后处理队列中第一个任务
		if (tasks.empty())
			continue;
		lock.lock();
		Task task = tasks.front();
		tasks.pop();
		lock.unlock();

		// todo
		switch (task.type)
		{
			case Task::CONNECT:
				break;
			case Task::SENDSTREAM:
				break;
			case Task::SHUTDOWN:
				break;
			default:
				break;
		}
	}
}