//George Rush 2022 | github.com/gmrush

#include "server.h" 

Server::Server() {
    hostname = "localhost";
    port = "999";
    password = "";
    prefix = '[' + hostname + ':' + port + "] ";
}

Server::Server(std::string hostname, std::string port, std::string password) {
    this->hostname = hostname;
    this->port = port;
    this->password = password;
    prefix = '[' + hostname + ':' + port + "] ";
}

void Server::run() {
    std::cout << "[INFO] Loaded server with hostname \"" << hostname << "\" on port " << port << '\n';

    std::cout << "[INFO] Starting WSA" << '\n';
    int cresult;
    WSADATA wsadata;
    cresult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (cresult != 0) {
        std::cout << "[ERR] WSA startup failed, exiting" << '\n';
        return;
    }

    struct addrinfo *result = NULL, *ptr = NULL, info;
    ZeroMemory(&info, sizeof(info));
    info.ai_family = AF_INET; //IPV4
    info.ai_socktype = SOCK_STREAM;
    info.ai_protocol = IPPROTO_TCP; //Use TCP
    info.ai_flags = AI_PASSIVE;

    std::cout << "[INFO] Resolving local information" << '\n';
    cresult = getaddrinfo(NULL, port.c_str(), &info, &result); //Resolve local info
    if (cresult != 0) {
        std::cout << "[ERR] Could not resolve local address/port, exiting" << '\n';
        WSACleanup();
        return;
    }

    std::cout << "[INFO] Creating socket" << '\n';
    sock = INVALID_SOCKET;
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol); //Init socket
    if (sock == INVALID_SOCKET) {
        std::cout << "[ERR] Socket could not be created, exiting" << '\n';
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    std::cout << "[INFO] Binding socket" << '\n';
    cresult = bind(sock, result->ai_addr, (int) result->ai_addrlen); //Bind socket
    if (cresult == SOCKET_ERROR) {
        std::cout << "[ERR] Failed to bind socket, exiting" << '\n';
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return;
    }
    freeaddrinfo(result); //Address info no longer needed

    std::cout << "[INFO] Attempting to listen on socket" << '\n';
    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "[ERR] Could not listen on socket, exiting" << '\n';
        std::cout << WSAGetLastError() << '\n';
        closesocket(sock);
        WSACleanup();
        return;
    }

    std::cout << "[INFO] Successfully started server with hostname \"" << hostname << "\" on port " << port << '\n';

    while (true) servListen(); //Listen on server

    closesocket(sock); //Final cleanup
    WSACleanup();

}

void Server::servListen() {
    std::cout << prefix << "Listening for connections..." << '\n';
    int cResult, cSendResult; //Send/receive result info
    int recvbuflen = DEFBUFLEN; //Length of receiving buffer
    char recvbuf[DEFBUFLEN]; //Receiving buffer
    SOCKADDR_IN cAddr; //Structure of incoming connection
    int cAddrlen = sizeof(cAddr); //Size of structure of incoming connection
    bool valid = false;

    SOCKET clientAccept;
    clientAccept = accept(sock, (SOCKADDR*) &cAddr, &cAddrlen); //Accept and record info from incoming connection
    if (clientAccept == INVALID_SOCKET) {
        std::cout << "[ERR] Failed to accept connection, exiting" << '\n';
        closesocket(sock);
        return;
    }

    std::cout << prefix << "Received connection from " << inet_ntoa(cAddr.sin_addr) << '\n'; //Display incoming address

    cResult = 1;
    while(cResult > 0) { //Main listening loop, supports one connection only

        cResult = recv(clientAccept, recvbuf, recvbuflen, 0);
        if (cResult > 0) {

            if (valid == false) { //Check connection packet is valid
                for (int i = 0; i < 16; i++) {
                    if ((uint8_t) recvbuf[i] != packetConnect[i]) {
                        std::cout << prefix << "Client connection rejected" << '\n';
                        closesocket(clientAccept);
                        return;
                    }
                }
                valid = true;
                std::cout << prefix << "Client connection validated, listening..." << '\n';

                int sendlength = hostname.length(); //Send client the hostname of server
                char* sendbuf = new char[sendlength];
                strcpy(sendbuf, hostname.c_str());
                send(clientAccept, sendbuf, sendlength, 0);
                delete[] sendbuf;

            } else {
                std::cout << prefix << "Bytes received: " << cResult << ": ";
                for (int i = 0; i < cResult; i++) std::cout << recvbuf[i];
                std::cout << '\n';
                
                cSendResult = send(clientAccept, recvbuf, cResult, 0);
                if (cSendResult == SOCKET_ERROR) {
                    std::cout << prefix << "Failed to send data, closing connection" << '\n';
                    closesocket(clientAccept);
                    return;
                }
                std::cout << prefix << "Bytes sent: " << cSendResult << '\n';
            }

        } 
        else if (cResult == 0) std::cout << prefix << "Connection was closed by client" << '\n';
        else {
            std::cout << prefix << "Receiving failed, closing connection (client likely dropped)" << '\n';
            closesocket(clientAccept);
            return;
        }

    }

}
