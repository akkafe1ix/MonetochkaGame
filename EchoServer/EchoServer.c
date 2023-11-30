#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 45693;
const int BUFFER_SIZE = 4096;

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Создание сокета
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

    std::cout << "Ожидание подключения..." << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket;

    // Принятие входящего подключения
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket == -1) {
        std::cerr << "Ошибка при принятии подключения" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Клиент №1 подключен" << std::endl;

    const char* message = "Вы Игрок1 ;)";
    send(clientSocket, message, strlen(message), 0);
    message = "Ожидайте пока противник подключится и сделает свой ход";
    send(clientSocket, message, strlen(message), 0);

    socklen_t clientAddrLen2 = sizeof(clientAddr);
    int clientSocket2;

    // Принятие входящего подключения
    clientSocket2 = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen2);

    if (clientSocket2 == -1) {
        std::cerr << "Ошибка при принятии подключения" << std::endl;
        close(serverSocket);
        return -1;
    }
    std::cout << "Клиент №2 подключен" << std::endl;

    message = "Вы Игрок2 ;)";
    send(clientSocket2, message, strlen(message), 0);

    char buffer1[BUFFER_SIZE];
    int bytesRead1;
    char buffer2[BUFFER_SIZE];
    int bytesRead2;

    // Прием данных от клиента и отправка обратно
    while (true) {
        message = "Сделайте выбор в какую руку положить монетку (1/2)";
        send(clientSocket2, message, strlen(message), 0);

        bytesRead2 = recv(clientSocket2, buffer2, sizeof(buffer2), 0);
        std::cout << "Получено от клиента №2: " << std::string(buffer2, bytesRead2) << std::endl;

        message = "Ожидаем выбор противника..";
        send(clientSocket2, message, strlen(message), 0);

        message = "Противник сделал свой ход...";
        send(clientSocket, message, strlen(message), 0);
        message = "Угадайте в какую руку противник положил монетку (1/2)";
        send(clientSocket, message, strlen(message), 0);

        bytesRead1 = recv(clientSocket, buffer1, sizeof(buffer1), 0);
        std::cout << "Получено от клиента №2: " << std::string(buffer1, bytesRead1) << std::endl;
        int comparisonResult = memcmp(buffer1, buffer2, std::min(bytesRead1, bytesRead2));
        if (comparisonResult == 0) {
            message = "Вы выйграли!";
            send(clientSocket, message, strlen(message), 0);
            message = "Вы проиграли :(";
            send(clientSocket2, message, strlen(message), 0);
        }
        else {
            message = "Вы выйграли!";
            send(clientSocket2, message, strlen(message), 0);
            message = "Вы проиграли :(";
            send(clientSocket, message, strlen(message), 0);
        }
        message = "Ожидайте пока противник сделает свой ход";
        send(clientSocket, message, strlen(message), 0);
    }
    // Закрытие соединения
    close(clientSocket);
    close(clientSocket2);
    close(serverSocket);

    return 0;
}
