#include <arpa/inet.h>
#include <atomic>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#ifndef TCP_CLIENT_TCP_SERVER_H
#define TCP_CLIENT_TCP_SERVER_H

class TCPServer {
private:
    int port;
    std::mutex mtx;
    std::atomic<bool> shouldStop;

public:
    TCPServer() : shouldStop(false), port(0) {}
    explicit TCPServer(int port) : port(port), shouldStop(false) {}

    void run() {
        sockaddr_in serverAddr{};
        sockaddr_in client{};
        socklen_t clientSize = sizeof(client);

        int sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock < 0) {
            std::cerr << "Can't create a socket!" << std::endl;
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);

        inet_pton(AF_INET, "0.0.0.0", &serverAddr.sin_addr);

        if (bind(sock, (sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Unable to bind socket" << std::endl;
            close(sock);
            return;
        }

        if (listen(sock, SOMAXCONN) < 0) {
            std::cerr << "Can't listen!";
            return;
        }

        while (!shouldStop.load()) {
            // To wait for activity on the socket with a timeout of 1 second
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);

            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int activity = select(sock + 1, &readfds, nullptr, nullptr, &timeout);

            if (activity == -1) {
                std::cerr << "Error in select()" << std::endl;
                continue;
            }

            if (activity == 0) {
                continue;
            }

            mtx.lock();
            int clientSocket = accept(sock, (struct sockaddr *) &client, &clientSize);
            mtx.unlock();

            if (clientSocket < 0) {
                std::cerr << "Unable to accept connection" << std::endl;
                continue;
            }

            std::thread(&TCPServer::handleClient, this, clientSocket).detach();
        }

        close(sock);
    }

    void stop() {
        shouldStop.store(true);
    }

    void setPort(int newPort) {
        this->port = newPort;
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
