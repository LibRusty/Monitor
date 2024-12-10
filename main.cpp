#include "monitor.h"

void providerFunction(Monitor& monitor) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        monitor.provide();
    }
}

void consumerFunction(Monitor& monitor) {
    while (true) {
        monitor.consume();
    }
}

int main() {
    Monitor monitor;

    std::thread provider(providerFunction, std::ref(monitor));
    std::thread consumer(consumerFunction, std::ref(monitor));

    provider.join();
    consumer.join();

    return 0;
}
