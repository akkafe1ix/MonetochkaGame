#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <string>
#include <signal.h>
#include <algorithm>

const int PORT = 45693;
const int BUFFER_SIZE = 4096;
//g++ -std=c++11 -pthread Monetochka_copy.cpp -o Monetochka_copy
volatile sig_atomic_t flag = 0;
struct ClientInfo {
    int socket;
    std::string name;

    ClientInfo(int socket_, const std::string& name_) : socket(socket_), name(name_) {}
};
void sigintHandler(int sig) {
    flag = 1;
}

void closeSockets(const std::vector<int>& sockets) {
    for (int socket : sockets) {
        close(socket);
    }
}
void clientHandler(ClientInfo client, std::vector<ClientInfo>& clients) {
    // Обработка подключения клиента
    char buffer[1024];
    int valread;

    while (true) {
        valread = recv(client.socket, buffer, sizeof(buffer), 0);
        buffer[valread] = '\0';
        if (valread <= 0) {
            auto it = std::find_if(clients.begin(), clients.end(), [&client](const ClientInfo& c) {
                return c.socket == client.socket;
            });
            if (it != clients.end()) {
                std::cout << "Client disconnected: " << it->name << std::endl;
                clients.erase(it);
            }
            close(client.socket);
            break;
        }
        std::string receivedMessage(buffer, valread);
        std::cout << client.name << " sent: " << receivedMessage << std::endl;
        for (const ClientInfo& otherClient : clients) {
            if (otherClient.socket != client.socket) {
                std::string message = client.name + ": " + std::string(buffer, valread);
                send(otherClient.socket, message.c_str(), message.size(), 0);
            }
        }
    }
}
void handleClientConnection(int clientSocket, std::vector<ClientInfo>& clients) {
    char nameBuffer[1024];
    int nameSize = recv(clientSocket, nameBuffer, sizeof(nameBuffer), 0);
    nameBuffer[nameSize] = '\0';

    std::string clientName(nameBuffer);
    bool nameExists = false;//уникальность
    for (const ClientInfo& client : clients) {
        if (client.name == clientName) {
            nameExists = true;
            break;
        }
    }
    if(nameExists){
        send(clientSocket, "false", 6, 0);
        close(clientSocket);
    }
    send(clientSocket, "true", 5, 0);
    std::cout << "New client connected: " << clientName << std::endl;

    clients.emplace_back(clientSocket, clientName);

    std::thread clientThread([&clients, clientSocket]() {
        clientHandler(clients.back(), clients);
    });

    clientThread.detach();
}


int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;
    std::vector<ClientInfo> clients; // Список клиентов

    #pragma region Создание сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка при создании сокета" << std::endl;
        return -1;
    }

    // Настройка параметров сервера
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Ошибка при привязке сокета к адресу" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Сервер ожидает подключений
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Ошибка при прослушивании порта" << std::endl;
        close(serverSocket);
        return -1;
    }
#pragma endregion
    std::cout << "Ожидание подключения..." << std::endl;
    while(!flag){
        addrSize = sizeof(clientAddr);

        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize)) < 0) {
            perror("Acceptance failed");
            return -1;
        }
        
        handleClientConnection(clientSocket, clients);
    }
    close(serverSocket);

    return 0;
}

