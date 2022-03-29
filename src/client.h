//George Rush 2022 | github.com/gmrush

#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"

#define NUMCOMMANDS 6

const std::string commands[] = 
    {"\thelp: Displays commands and usages",
    "\twhoami: Displays current server",
    "\tconnect <ip> <port>: Connected to a specified Cleareye server",
    "\tdisconnect: Disconnects from the server",
    "\texit: Terminates client session",
    "\techo: Returns a message from the server"};

class Client {
public:
    SOCKET clientSocket;
    SOCKET tempSocket;
    std::string prefix;
    std::string hostname;

    Client();
    void run();
    void parseCommand(std::string);
    void clientConnect(std::string, std::string);
    void clientSend(std::string);
    void closeConnection();
    void clientValidate();

};

#endif //CLIENT_H
