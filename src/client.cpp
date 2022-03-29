//George Rush 2022 | github.com/gmrush

#include "client.h"

Client::Client() {
    clientSocket = INVALID_SOCKET;
    tempSocket = INVALID_SOCKET;
    prefix = "";
    hostname = "";
}

void Client::run(){
    int cresult;
    WSADATA wsadata;
    cresult = WSAStartup(MAKEWORD(1, 1), &wsadata); //Init WSA
    if (cresult != 0) {
        std::cout << "[ERR] WSA startup failed, exiting" << '\n';
        return;
    }

    std::string in;
    std::cout << "[INFO] Use 'help' for help" << '\n';
    while (true) {
        clientSocket = tempSocket;
        if (clientSocket == INVALID_SOCKET) prefix = "[-] ";
        std::cout << prefix;
        std::getline(std::cin, in);
        if (in == "exit") break;
        parseCommand(in);
    }

    closeConnection();
    WSACleanup();
} 

void Client::parseCommand(std::string in) {
    std::vector<std::string> part;
    std::stringstream ssin(in);
    std::string str;

    if (in.empty()) return;
    while (ssin.good()) {
        ssin >> str;
        part.push_back(str);
    }

    if (part[0] == "help") {
        for (int i = 0; i < NUMCOMMANDS; i++) std::cout << commands[i] << '\n';
        return;
    }
    if (part[0] == "whoami") {
        std::cout << hostname << '\n';
        return;
    }
    if (part[0] == "connect") {
        if (part.size() != 3) return;
        clientConnect(part[1], part[2]);
        return;
    }
    if (part[0] == "disconnect") {
        if (clientSocket == INVALID_SOCKET) return;
        std::cout << prefix << "Disconnecting..." << '\n';
        closeConnection();
        return;
    }
    if (part[0] == "echo") {
        if (clientSocket == INVALID_SOCKET) return;
        if (part.size() < 2) return;
        std::string str;
        for (int i = 1; i < part.size(); i++) str += part[i] + ' ';
        clientSend(str);
        return;
    }
    std::cout << "[INFO] That is not a valid command" << '\n';
}

void Client::clientConnect(std::string address, std::string port) {
    std::cout << prefix <<  "Connecting..." << '\n';

    int cresult;
    struct addrinfo *result = NULL, *ptr = NULL, info;

    ZeroMemory(&info, sizeof(info));
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP;

    cresult = getaddrinfo(address.c_str(), port.c_str(), &info, &result);
    if (cresult != 0) {
        std::cout << "[ERR] Could not resolve address/port" << '\n';
        return;
    }

    SOCKET clientSocket = INVALID_SOCKET;
    ptr = result;
    clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "[ERR] Socket could not be created" << '\n';
        freeaddrinfo(result);
        return;
    }

    cresult = connect(clientSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
    if (cresult == SOCKET_ERROR) {
        std::cout << "[ERR] Socket error, could not connect" << '\n';
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return;
    }
    freeaddrinfo(result);

    tempSocket = clientSocket;

    clientValidate(); //Send valid packet to establish connection, receive server info

    prefix = '[' + hostname + ':' + port + "] ";
    std::cout << prefix << "Connected!" << '\n';
}

void Client::clientValidate() {
    clientSocket = tempSocket;

    int cresult;
    char recvbuf[DEFBUFLEN];
    std::string name;
    
    send(clientSocket, (char*) packetConnect, 16, 0);
    cresult = recv(clientSocket, recvbuf, DEFBUFLEN, 0);
    if (cresult != 0) {
        for (int i = 0; i < cresult; i++) name += recvbuf[i];
        hostname = name;
    }
}

void Client::clientSend(std::string data) {
    int cresult;
    int sendlength = data.length();
    char* sendbuf = new char[sendlength];
    strcpy(sendbuf, data.c_str());
    char recvbuf[DEFBUFLEN];
    std::cout << "[INFO] Sending data..." << '\n';

    cresult = send(clientSocket, sendbuf, sendlength, 0);
    if (cresult == SOCKET_ERROR) {
        std::cout << "[ERR] Sending failed, disconnecting" << '\n';
        closesocket(clientSocket);
        closesocket(tempSocket);
        clientSocket = INVALID_SOCKET;
        tempSocket = INVALID_SOCKET;
        return;
    }
    std::cout << "[INFO] Bytes sent: " << cresult << '\n';
    delete[] sendbuf;

    cresult = recv(clientSocket, recvbuf, DEFBUFLEN, 0);
    if (cresult > 0) {
        std::cout << "[INFO] Bytes received: " << cresult << ": ";
        for (int i = 0; i < cresult; i++) std::cout << recvbuf[i];
        std::cout << '\n';
    }
    else if (cresult == 0) std::cout << "[INFO] Connection was closed by server" << '\n';
    else std::cout << "[ERR] Receiving failed" << '\n';
}

void Client::closeConnection() {
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
    shutdown(tempSocket, SD_BOTH);
    closesocket(tempSocket);
    tempSocket = INVALID_SOCKET;
}
