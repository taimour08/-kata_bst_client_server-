#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits>

using namespace std;

class BSTClient {
public:
    void run(const std::string& serverIP, int port) {
        int clientSocket = connectToServer(serverIP, port);
        std::cout << "Connected to server: " << serverIP << ":" << port << std::endl;

        while (true) {
            std::string command;
            std::cout << "Enter command: ";
            getline(cin, command);

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

private:
    int connectToServer(const std::string& serverIP, int port) {
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Create client socket
        if (clientSocket == -1) {
            throw std::runtime_error("Failed to create client socket.");
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);

        if (inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
            throw std::runtime_error("Invalid server IP address.");
        }

        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
            throw std::runtime_error("Failed to connect to server.");
        }

        return clientSocket; // Return the client socket
    }
};

// Function to send a command to the server
void sendCommand(int serverSocket, const std::string& command) {
    send(serverSocket, command.c_str(), command.size(), 0);
}

// Function to receive and print the server's response
void receiveResponse(int serverSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    } else if (bytesRead == 0) {
        std::cout << "Server closed the connection" << std::endl;
        close(serverSocket);
        exit(0);
    } else {
        throw std::runtime_error("Failed to receive data from server");
    }
}

// Client program entry point
int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create server socket.");
    }

    // Server details
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1234); // Replace with the server's port number

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        throw std::runtime_error("Invalid address or address not supported.");
    }

    // Connect to the server
    if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        throw std::runtime_error("Failed to connect to server.");
    }

    // Send commands to the server
    std::string command;
    while (true) {
        try {
            std::cout << "Enter command (insert, find, delete, or exit): ";
            std::getline(std::cin, command);

            // Send the command to the server
            sendCommand(serverSocket, command);

            // Receive and print the server's response
            receiveResponse(serverSocket);

            // Handle termination condition
            if (command == "exit") {
                std::cout << "Exiting the client program" << std::endl;
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            close(serverSocket);
            exit(1);
        }
    }

    close(serverSocket);
    return 0;
}
