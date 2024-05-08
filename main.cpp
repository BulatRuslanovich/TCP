//#include <iostream>
//
//#include <sys/types.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <string>
//#include <cstring>
//
//int main() {
//    int sock = socket(AF_INET, SOCK_STREAM, 0);
//
//    if (sock == -1) {
//        std::cerr << "Can't create a socket!";
//        return -1;
//    }
//
//    int port = 54077;
//    std::string ipAddress = "127.0.0.1";
//
//    sockaddr_in hint;
//    hint.sin_family = AF_INET;
//    hint.sin_port = htons(port);
//    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
//
//    int connectRes = connect(sock, (sockaddr*)&hint, sizeof (hint));
//
//    if (connectRes == -1) {
//        return -2;
//    }
//
//    char buf[4096];
//    std::string userInput;
//
//    do {
//        std::cout << "> ";
//        getline(std::cin, userInput);
//
//        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
//        if (sendRes == -1) {
//            std::cout << "Could not send to server!" << std::endl;
//            continue;
//        }
//
//        memset(buf, 0, 4096);
//        int byteReceived = recv(sock, buf, 4096, 0);
//        std::cout << "SERVER> " << std::string(buf, byteReceived) << std::endl;
//    } while (true);
//
//    close(sock);
//    return 0;
//}

#include <string>
#include <iostream>
#include <sstream>
#include <thread>

#include "TCP_client.h"
#include "TCP_server.h"


int main(int argc, char *argv[]) {
    std::string input;
    bool isServerStart = false;
    std::thread serverThread;

    while (true) {
        std::cout << "> ";
        getline(std::cin, input);

        if (input.substr(0, 6) == "server") {
            int port;
            std::istringstream iss(input);
            std::string keyWord;
            iss >> keyWord;
            iss >> port;

            TCPServer server(port);
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

                TCPClient client(&name, clientPort, period);
                std::thread clientThread(&TCPClient::run, &client);
                clientThread.detach();
            } else {
                std::cerr << "Server not started!" << std::endl;
            }
        } else if (input == "exit") {
            break;
        }
    }

    if (serverThread.joinable()) {
        serverThread.join();
    }

    return 0;
}

