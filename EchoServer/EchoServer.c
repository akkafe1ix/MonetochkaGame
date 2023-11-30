#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

const int PORT = 45693;
const int BUFFER_SIZE = 4096;

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // �������� ������
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "������ ��� �������� ������" << std::endl;
        return -1;
    }

    // ��������� ���������� �������
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // �������� ������ � ������ � �����
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "������ ��� �������� ������ � ������" << std::endl;
        close(serverSocket);
        return -1;
    }

    // ������ ������� �����������
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "������ ��� ������������� �����" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "�������� �����������..." << std::endl;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket;

    // �������� ��������� �����������
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket == -1) {
        std::cerr << "������ ��� �������� �����������" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "������ �1 ���������" << std::endl;

    const char* message = "�� �����1 ;)";
    send(clientSocket, message, strlen(message), 0);
    message = "�������� ���� ��������� ����������� � ������� ���� ���";
    send(clientSocket, message, strlen(message), 0);

    socklen_t clientAddrLen2 = sizeof(clientAddr);
    int clientSocket2;

    // �������� ��������� �����������
    clientSocket2 = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen2);

    if (clientSocket2 == -1) {
        std::cerr << "������ ��� �������� �����������" << std::endl;
        close(serverSocket);
        return -1;
    }
    std::cout << "������ �2 ���������" << std::endl;

    message = "�� �����2 ;)";
    send(clientSocket2, message, strlen(message), 0);

    char buffer1[BUFFER_SIZE];
    int bytesRead1;
    char buffer2[BUFFER_SIZE];
    int bytesRead2;

    // ����� ������ �� ������� � �������� �������
    while (true) {
        message = "�������� ����� � ����� ���� �������� ������� (1/2)";
        send(clientSocket2, message, strlen(message), 0);

        bytesRead2 = recv(clientSocket2, buffer2, sizeof(buffer2), 0);
        std::cout << "�������� �� ������� �2: " << std::string(buffer2, bytesRead2) << std::endl;

        message = "������� ����� ����������..";
        send(clientSocket2, message, strlen(message), 0);

        message = "��������� ������ ���� ���...";
        send(clientSocket, message, strlen(message), 0);
        message = "�������� � ����� ���� ��������� ������� ������� (1/2)";
        send(clientSocket, message, strlen(message), 0);

        bytesRead1 = recv(clientSocket, buffer1, sizeof(buffer1), 0);
        std::cout << "�������� �� ������� �2: " << std::string(buffer1, bytesRead1) << std::endl;
        int comparisonResult = memcmp(buffer1, buffer2, std::min(bytesRead1, bytesRead2));
        if (comparisonResult == 0) {
            message = "�� ��������!";
            send(clientSocket, message, strlen(message), 0);
            message = "�� ��������� :(";
            send(clientSocket2, message, strlen(message), 0);
        }
        else {
            message = "�� ��������!";
            send(clientSocket2, message, strlen(message), 0);
            message = "�� ��������� :(";
            send(clientSocket, message, strlen(message), 0);
        }
        message = "�������� ���� ��������� ������� ���� ���";
        send(clientSocket, message, strlen(message), 0);
    }
    // �������� ����������
    close(clientSocket);
    close(clientSocket2);
    close(serverSocket);

    return 0;
}
