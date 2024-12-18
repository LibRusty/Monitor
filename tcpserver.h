#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

class TcpServer {
public:
    TcpServer(int port);
    ~TcpServer();

    void start();

private:
    int port;
    int serverSocket;
    int clientSocket; 
    sigset_t origMask;
    volatile sig_atomic_t wasSigHup;

    void createSocket();
    void bindSocket();
    void listenForConnections();
    void handleSignal(int sig);
    void blockSignal();
    void handleNewConnection();
    void handleClientData();
    void handleSignalEvent();
};

#endif // TCPSERVER_H