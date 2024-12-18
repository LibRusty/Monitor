#include "TcpServer.h"

TcpServer::TcpServer(int port) : port(port), serverSocket(-1), clientSocket(-1), wasSigHup(0) {
    createSocket();
    bindSocket();
    listenForConnections();
    blockSignal();
}

TcpServer::~TcpServer() {
    close(serverSocket);
    if (clientSocket != -1) {
        close(clientSocket);
    }
}

void TcpServer::start() {
    while (true) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(serverSocket, &readFds);

        int maxFd = serverSocket;
        if (clientSocket != -1) {
            FD_SET(clientSocket, &readFds);
            if (clientSocket > maxFd) {
                maxFd = clientSocket;
            }
        }

        int activity = pselect(maxFd + 1, &readFds, nullptr, nullptr, nullptr, &origMask);
        if (activity == -1 && errno == EINTR) {
            handleSignalEvent();
        } else if (activity > 0) {
            if (FD_ISSET(serverSocket, &readFds)) {
                handleNewConnection();
            }
            if (clientSocket != -1 && FD_ISSET(clientSocket, &readFds)) {
                handleClientData();
            }
        }
    }
}

void TcpServer::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }
}

void TcpServer::bindSocket() {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }
}

void TcpServer::listenForConnections() {
    if (listen(serverSocket, 1) == -1) {
        perror("Failed to listen for connections");
        exit(EXIT_FAILURE);
    }
}

void TcpServer::handleSignal(int sig) {
    if (sig == SIGHUP) {
        wasSigHup = 1;
    }
}

void TcpServer::blockSignal() {
    sigset_t blockedMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, SIGHUP);
    sigprocmask(SIG_BLOCK, &blockedMask, &origMask);
}

void TcpServer::handleNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        perror("Failed to accept connection");
        return;
    }

    std::cout << "New connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;
}

void TcpServer::handleClientData() {
    char buffer[1024];
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead == 0) {
        std::cout << "Client disconnected" << std::endl;
        close(clientSocket);
        clientSocket = -1;
    } else if (bytesRead > 0) {
        std::cout << "Received " << bytesRead << " bytes from client" << std::endl;
    } else {
        perror("Failed to read from client");
    }
}

void TcpServer::handleSignalEvent() {
    if (wasSigHup) {
        std::cout << "SIGHUP signal received" << std::endl;
        wasSigHup = 0;
    }
}