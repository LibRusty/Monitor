#include "TcpServer.h"

int main() {
    // Создаем объект сервера
    TcpServer server(12345);

    // Запускаем сервер
    server.start();

    return 0;
}