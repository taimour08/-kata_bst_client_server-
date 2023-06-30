#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


class BSTClient {
public:
    void run(const std::string& serverIP, int port) {
        int clientSocket = connectToServer(serverIP, port);
        std::cout << "Connected to server: " << serverIP << ":" << port << std::endl;

        while (true) {
            std::string command;
            std::cout << "Enter command: ";
            std::getline(std::cin, command);

            if (command == "exit") {
                break;
            }

            send(clientSocket, command.c_str(), command.length(), 0); // Send command to the server

            char buffer[1024];
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Receive response from the server
            if (bytesRead <= 0) {
                break;
            }

            buffer[bytesRead] = '\0';
            std::cout << "Server response: " << buffer << std::endl; // Print server's response
        }

        close(clientSocket);
        std::cout << "Disconnected from server." << std::endl;
    }


};

int main() {
    BSTClient client;
    client.run("127.0.0.1", 12345); // Connect to the server at IP address "127.0.0.1" and port 12345

    return 0;
}

