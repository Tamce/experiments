#pragma once
#include <winsock2.h>
#include "Exception.h"

namespace tmc
{
    static class Socket
    {
    private:
        SOCKET listener;
        WSADATA wsaData;
    public:
        Socket();
        Socket(const char *port);
        void init(const char *port);
        void listen(int maxQueue = SOMAXCONN);
        SOCKET *accept();
        void clean();
    };
}

void tmc::Socket::clean()
{
    closesocket(listener);
    WSACleanup();
}

SOCKET *tmc::Socket::accept()
{
    static SOCKET client;
    client = INVALID_SOCKET;
    client = accept(listener, NULL, NULL);
    if (INVALID_SOCKET == client)
    {
        clean();
        throw Exception("Error in accept()!", WSAGetLastError());
    }
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
        clean();
        throw Exception("Initialize WinSock v2.2 Failed", result);
    }

    addrinfo *addrResult = nullptr, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocal = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Getting address info
    result = getaddrinfo(nullptr, port, &hints, &addrResult);
    if (0 != result)
    {
        clean();
        throw Exception("Get Addrinfo Failed!", result);
    }

    // Creating a listen socket
    SOCKET *listener = INVALID_SOCKET;
    *listener = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocal);
    if (INVALID_SOCKET == *socketConnection)
    {
        freeaddrinfo(addrResult);
        clean();
        throw Exception("Error while creating a socket!", WSAGetLastError());
    }

    // Binding a socket to port
    result = bind(listener, addrResult->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(addrResult);
    if (SOCKET_ERROR == result)
    {
        clean();
        throw Exception("Error in bind()!", WSAGetLastError());
    }
}

void tmc::Socket::listen(SOCKET *s, int maxQueue)
{
    if (SOCKET_ERROR == listen(s, maxQueue))
    {
        clean();
        throw Exception("Error in listen()!", WSAGetLastError());
    }
}
