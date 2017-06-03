#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Exception.h"

namespace tmc
{
    class ClientSocket;

    class Socket
    {
    private:
        SOCKET listener;
        WSADATA wsaData;
    public:
        Socket();
        Socket(const char *port);
        ~Socket(){ clean(); }
        void init(const char *port);
        void listen(int maxQueue = SOMAXCONN);
        ClientSocket accept();
        void clean();
    };

    class ClientSocket
    {
        SOCKET socket;
    public:
        ClientSocket(SOCKET s):socket(s){}
        ~ClientSocket()
        {
			shutdown(socket, SD_SEND);
			closesocket(socket);
        }

        int gets(char *buffer, int len)
        {
            return recv(socket, buffer, len, 0);
        }

        int get(char &c)
        {
            return recv(socket, &c, 1, 0);
        }
    };
}

void tmc::Socket::clean()
{
    closesocket(listener);
    WSACleanup();
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

tmc::Socket::Socket():listener(INVALID_SOCKET){}

tmc::Socket::Socket(const char *port):listener(INVALID_SOCKET)
{
    init(port);
}

void tmc::Socket::init(const char *port)
{
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    // Use WinSock v2.2 to initialize wsaData
    if (0 != result)
    {
        // initialization failed
        throw Exception("Initialize WinSock v2.2 Failed", result);
    }


    addrinfo *addrResult = nullptr, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Getting address info
    result = getaddrinfo(nullptr, port, &hints, &addrResult);
    if (0 != result)
    {
        throw Exception("Get Addrinfo Failed!", result);
    }

    // Creating a listen socket
    listener = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (INVALID_SOCKET == listener)
    {
        throw Exception("Error while creating a socket!", WSAGetLastError());
    }

    // Binding a socket to port
    result = bind(listener, addrResult->ai_addr, (int)addrResult->ai_addrlen);
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

