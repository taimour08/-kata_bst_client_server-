#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class BSTNode {
public:
    int key;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int key) : key(key), left(nullptr), right(nullptr) {}
};


class BSTServer {
public:
    void run(int port) {
        int serverSocket = createServerSocket(port);
        std::cout << "Server started. Listening on port " << port << "." << std::endl;

        while (true) {
            int clientSocket = acceptClientConnection(serverSocket);
            std::cout << "Client connected. Client socket: " << clientSocket << std::endl;

            handleClientRequests(clientSocket);

            close(clientSocket);
            std::cout << "Client disconnected. Client socket: " << clientSocket << std::endl;
        }
    }

private:
    BSTNode* root; // Binary search tree root

    int createServerSocket(int port) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Create server socket
        if (serverSocket == -1) {
            throw std::runtime_error("Failed to create server socket.");
        }

        sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
            throw std::runtime_error("Failed to bind server socket to port.");
        }

        if (listen(serverSocket, 3) < 0) {
            throw std::runtime_error("Failed to listen on server socket.");
        }

        return serverSocket;
    }

  int acceptClientConnection(int serverSocket) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength); // Accept client connection
        if (clientSocket < 0) {
            throw std::runtime_error("Failed to accept client connection.");
        }

        return clientSocket;
    }

     void handleClientRequests(int clientSocket) {
        char buffer[1024];
        std::string clientMessage;

        while (true) {
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // Receive client's message
            if (bytesRead <= 0) {
                break;
            }

            buffer[bytesRead] = '\0';
            clientMessage += buffer;

            if (clientMessage.find('\n') != std::string::npos) {
                std::string response = processClientCommand(clientMessage); // Process client's command
                send(clientSocket, response.c_str(), response.length(), 0); // Send response to client

                clientMessage.clear();
            }
        }
    }


      std::string processClientCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string operation, value;
        iss >> operation >> value;

        if (operation == "insert") {
            int num = std::stoi(value);
            if (insertNode(root, num)) {
                return ""; // Success, no error message
            } else {
                return "ERROR: " + value + " already exists in BST\n"; // Key already exists in the BST
            }
        } else if (operation == "delete") {
            int num = std::stoi(value);
            if (deleteNode(root, num)) {
                return ""; // Success, no error message
            } else {
                return "ERROR: " + value + " not found in BST\n"; // Key not found in the BST
            }
        } else if (operation == "find") {
            int num = std::stoi(value);
            if (findNode(root, num)) {
                return "found\n"; // Key found in the BST
            } else {
                return "not found\n"; // Key not found in the BST
            }
        } else {
            return "ERROR: Invalid command\n"; // Invalid command
        }
    }

}

int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Bind socket to a specific port
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);  // Choose any port number

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to bind socket to port." << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 1) == -1) {
        std::cerr << "Failed to listen for connections." << std::endl;
        return 1;
    }

    std::cout << "Server is listening for connections..." << std::endl;

    // Accept a client connection
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        std::cerr << "Failed to accept client connection." << std::endl;
        return 1;
    }

    // Receive number from client
    int number;
    if (recv(clientSocket, &number, sizeof(number), 0) == -1) {
        std::cerr << "Failed to receive number from client." << std::endl;
        return 1;
    }

    std::cout << "Received number from client: " << number << std::endl;

    // Close sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
