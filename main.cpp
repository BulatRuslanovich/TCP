#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "TCP_client.h"
#include "TCP_server.h"


int main(int argc, char *argv[]) {
    std::string input;
    bool isServerStart = false;
    std::thread serverThread;
    TCPServer server;

    while (true) {
        std::cout << "> ";
        getline(std::cin, input);

        if (input.substr(0, 6) == "server") {
            int port;
            std::istringstream iss(input);
            std::string keyWord;
            iss >> keyWord;
            iss >> port;

            server.setPort(port);
            serverThread = std::thread(&TCPServer::run, &server);
            isServerStart = true;
        } else if (input.substr(0, 6) == "client") {
            if (isServerStart) {
                std::istringstream iss(input);
                std::string keyWord;
                iss >> keyWord;
                std::string name;
                iss >> name;
                int clientPort;
                iss >> clientPort;
                int period;
                iss >> period;

                auto *client = new TCPClient(name, clientPort, period);
                std::thread clientThread(&TCPClient::run, client);
                clientThread.detach();
            } else {
                std::cerr << "Server not started! \n > ";
            }
        } else if (input == "exit") {
            if (isServerStart) {
                server.stop();
                serverThread.join();

            } else {
                std::cout << "Server is not running." << std::endl;
            }

            std::cout << "Exiting program..." << std::endl;
            break;
        }
    }

    return 0;
}
