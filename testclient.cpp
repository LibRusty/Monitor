#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    const char* server_ip = "127.0.0.1"; 
    const int server_port = 12345;       

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create socket");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_socket);
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to server " << server_ip << ":" << server_port << std::endl;

    const char* message = "Hello, Server!";
    std::cout << "Sending message: " << message << std::endl;

    if (send(client_socket, message, strlen(message), 0) < 0) {
        perror("Failed to send message");
        close(client_socket);
        return 1;
    }

    close(client_socket);
    std::cout << "Connection closed" << std::endl;

    return 0;
}