//
// Created by bulatruslanovich on 08/05/2024.
//

#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <utility>

#ifndef TCP_CLIENT_TCP_SERVER_H
#define TCP_CLIENT_TCP_SERVER_H


class TCPServer {
private:
    int port;
    std::mutex mtx;

public:
    explicit TCPServer(int port) : port(port) {}

    void run() {
        sockaddr_in serverAddr{};
        sockaddr_in client{};
        socklen_t clientSize = sizeof (client);

        int sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock < 0) {
            std::cerr << "Can't create a socket!" << std::endl;
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr);

        if (bind(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Unable to bind socket" << std::endl;
            close(sock);
            return;
        }

        if (listen(sock, SOMAXCONN) < 0) {
            std::cerr << "Can't listen!";
            return;
        }

        while (true) {
            mtx.lock();
            int clientSocket = accept(sock, (struct sockaddr *)&client, &clientSize);
            mtx.unlock();

            if (clientSocket < 0) {
                std::cerr << "Unable to accept connection" << std::endl;
                continue;
            }

            std::thread(&TCPServer::handleClient, this, clientSocket).detach();
        }
    }

private:
    void handleClient(int clientSocket) {
        char buffer[1024];
        int bytesRead;

        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            std::ofstream logfile("log.txt", std::ios_base::app | std::ios_base::out);
            logfile << std::string(buffer, bytesRead) << std::endl;
            logfile.close();
        }

        close(clientSocket);
    }
};

#endif//TCP_CLIENT_TCP_SERVER_H
