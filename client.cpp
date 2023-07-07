#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <arpa/inet.h>

// Function to send a command to the server
void sendCommand(int serverSocket, const std::string& command) {
    send(serverSocket, command.c_str(), command.size(), 0); // 2
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
    // Create socket - Communication channel.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create server socket.");
    }

    // Server details
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1234); // Replace with the server's port number

    // Convert IP address from text to binary form // 2
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
