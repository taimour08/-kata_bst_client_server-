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


int main() {
    BSTClient client;
    client.run("127.0.0.1", 12345); // Connect to the server at IP address "127.0.0.1" and port 12345

    return 0;
}
