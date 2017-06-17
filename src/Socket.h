#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Exception.h"
#include <cstring>
#include <utility>

namespace tmc
{
    class ClientSocket;

    class Socket
    {
    public:
		enum type{PASSIVE, POSITIVE};
        Socket();
        Socket(const char *port);
		~Socket()
		{
			closesocket(listener);
			WSACleanup();
		}
        void open(const char *port);
        void listen(int maxQueue = SOMAXCONN);
		void openListen(const char *port, int maxQueue = SOMAXCONN)
		{
			open(port);
			listen(maxQueue);
		}
        ClientSocket accept();
		int close();

		ClientSocket connect(const char *address, const char *port);
	private:
		SOCKET listener;
		WSADATA wsaData;
		addrinfo *getAddr(const char *address, const char *port, type t);
    };

	class ClientSocket
	{
		SOCKET socket;
	public:
		ClientSocket():ClientSocket(INVALID_SOCKET){}
        ClientSocket(SOCKET s):socket(s){}
        ClientSocket (const ClientSocket&) = delete;
        ClientSocket (ClientSocket &&s):ClientSocket(s.socket)
        {
        	s.socket = INVALID_SOCKET;
        }
        ClientSocket &operator=(const ClientSocket&) = delete;
        ClientSocket &operator=(ClientSocket &&s)
        {
        	shutdownClose();
        	socket = s.socket;
        	s.socket = INVALID_SOCKET;
        	return *this;
        }

        ~ClientSocket()
        {
			shutdown();
			closesocket(socket);
        }

		void close()
		{
			closesocket(socket);
			socket = INVALID_SOCKET;
		}

        int gets(char *buffer, int len)
        {
            return recv(socket, buffer, len, 0);
        }

        int get(char &c)
        {
            return recv(socket, &c, 1, 0);
        }

		int send(const char *buff)
		{
			return send(buff, std::strlen(buff));
		}

		int sendWithZero(const char *buff)
		{
			return send(buff, std::strlen(buff) + 1);
		}

		int send(const char *buff, int len)
		{
			return ::send(socket, buff, len, 0);
		}

		int shutdown(int how = SD_BOTH)
		{
			return ::shutdown(socket, how);
		}

		void shutdownClose(int how = SD_BOTH)
		{
			shutdown(how);
			close();
		}
    };
}

int tmc::Socket::close()
{
	return closesocket(listener);
}

tmc::ClientSocket tmc::Socket::accept()
{
    SOCKET client = INVALID_SOCKET;
    client = ::accept(listener, NULL, NULL);
    if (INVALID_SOCKET == client)
    {
        throw Exception("Error in accept()!", WSAGetLastError());
    }
    return ClientSocket(client);
}

tmc::Socket::Socket():listener(INVALID_SOCKET)
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Use WinSock v2.2 to initialize wsaData
	if (0 != result)
	{
		// initialization failed
		throw Exception("Initialize WinSock v2.2 Failed", result);
	}
}

tmc::Socket::Socket(const char *port):Socket()
{
    open(port);
}

addrinfo *tmc::Socket::getAddr(const char *address, const char *port, type passive = PASSIVE)
{
	addrinfo *addrResult = nullptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (passive == PASSIVE)
	{
		hints.ai_flags = AI_PASSIVE;
	}

	// Getting address info
	int result = getaddrinfo(address, port, &hints, &addrResult);
	if (0 != result)
	{
		throw Exception("Get Addrinfo Failed!", result);
	}
	return addrResult;
}

void tmc::Socket::open(const char *port)
{
	if (INVALID_SOCKET != listener)
	{
		throw Exception("Cannot open two port in one Socket object!");
	}
	addrinfo *addrResult = getAddr(nullptr, port, PASSIVE);

    // Creating a listen socket
    listener = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (INVALID_SOCKET == listener)
    {
        throw Exception("Error while creating a socket!", WSAGetLastError());
    }

    // Binding a socket to port
    int result = bind(listener, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    freeaddrinfo(addrResult);
    if (SOCKET_ERROR == result)
    {
        throw Exception("Error in bind()!", WSAGetLastError());
    }
}

void tmc::Socket::listen(int maxQueue)
{
	int result = ::listen(listener, maxQueue);
    if (SOCKET_ERROR == ::listen(listener, maxQueue))
    {
        throw Exception("Error in listen()!", WSAGetLastError());
    }
}

tmc::ClientSocket tmc::Socket::connect(const char *addr, const char *p)
{
	addrinfo *addrResult = getAddr(addr, p, POSITIVE);
	SOCKET client = INVALID_SOCKET;
	// Creating a connect socket
	client = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (INVALID_SOCKET == client)
	{
		throw Exception("Error while creating a socket!", WSAGetLastError());
	}

	// Connecting to target
	int result = ::connect(client, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	freeaddrinfo(addrResult);
	if (SOCKET_ERROR == result)
	{
		throw Exception("Error in connect()!", WSAGetLastError());
	}

	return ClientSocket(client);
}