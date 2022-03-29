//George Rush 2022 | github.com/gmrush

#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>
#include <stdio.h>
#include "packet.h"

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#elif UNIX
#include <sys.socket.h>
#endif

#define VERSION 0.1
#define AUTHOR "gmrush"
#define DEFBUFLEN 256 //Default buffer length in bytes

class Server {
public:
    std::string hostname;
    std::string port;
    std::string password;
    std::string prefix;
    SOCKET sock;

    Server();
    Server(std::string, std::string, std::string);
    void run();
    void servListen();
};

#endif //SERVER_H
