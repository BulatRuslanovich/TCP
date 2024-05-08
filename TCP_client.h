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

#ifndef TCP_CLIENT_TCP_CLIENT_H
#define TCP_CLIENT_TCP_CLIENT_H

class TCPClient {
private:
    std::string* clientName;
    int serverPort;
    int connectionPeriod;
    std::mutex mtx;

public:
    TCPClient(std::string *name, int port, int period) : clientName(name),  serverPort(port), connectionPeriod(period) {}

    void run() {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddr{};
        std::string ipAddress = "127.0.0.1";
        bool isConnected = false;

        if (sock < 0) {
            std::cerr << "Can't create a socket!" << std::endl;
            return;
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(serverPort);
        inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);



        while (true) {
            if (!isConnected) {
                if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
                    std::cerr << "Unable to connect to server" << std::endl;
                    close(sock);
                    return;
                }

                isConnected = true;
            }



            sendTimestamp(sock);
            std::this_thread::sleep_for(std::chrono::seconds(connectionPeriod));

        }
    }

private:

    void sendTimestamp(int sock) {
        time_t now = time(nullptr);
        char timestamp[50];
        strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S.000] ", localtime(&now));

        auto id = std::this_thread::get_id();
        std::stringstream ss;
        ss << id;
        std::string id_str = ss.str();

        std::string message = std::string(timestamp) + *clientName + " " + id_str.substr(0, 3);

        send(sock, message.data(), message.length(), 0);
    }
};

#endif//TCP_CLIENT_TCP_CLIENT_H
