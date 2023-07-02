#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include <sstream>

// Node structure for Binary Search Tree
struct Node {
    int data;
    Node* left;
    Node* right;

    explicit Node(int value) : data(value), left(nullptr), right(nullptr) {}
};

// Function to insert a value into the BST
void insertNode(Node*& root, int value) {
    if (root == nullptr) {
        root = new Node(value);
        return;
    }

    if (value < root->data) {
        insertNode(root->left, value);
    } else if (value > root->data) {
        insertNode(root->right, value);
    }
}

// Function to delete a value from the BST
bool deleteNode(Node*& root, int value) {
    if (root == nullptr) {
        return false;
    }

    if (value < root->data) {
        return deleteNode(root->left, value);
    } else if (value > root->data) {
        return deleteNode(root->right, value);
    } else {
        if (root->left == nullptr) {
            Node* temp = root;
            root = root->right;
            delete temp;
        } else if (root->right == nullptr) {
            Node* temp = root;
            root = root->left;
            delete temp;
        } else {
            Node* temp = root->right;
            while (temp->left != nullptr) {
                temp = temp->left;
            }
            root->data = temp->data;
            return deleteNode(root->right, temp->data);
        }
        return true;
    }
}

// Function to find a value in the BST
bool findNode(const Node* root, int value) {
    if (root == nullptr) {
        return false;
    }

    if (value < root->data) {
        return findNode(root->left, value);
    } else if (value > root->data) {
        return findNode(root->right, value);
    } else {
        return true;
    }
}

// Function to handle the client's commands
void handleCommand(int clientSocket, Node*& bstRoot, const std::string& command) {
    std::string operation;
    int value;

    std::cout << "Received command from client: " << command << std::endl;

    std::istringstream iss(command);
    iss >> operation >> value;

    std::string response;

    if (operation == "insert") {
        if (findNode(bstRoot, value)) {
            response = "Error: Value already exists in the BST.";
        } else {
            insertNode(bstRoot, value);
            response = "Insertion successful.";
        }
    } else if (operation == "delete") {
        if (deleteNode(bstRoot, value)) {
            response = "Deletion successful.";
        } else {
            response = "Error: Value not found in the BST.";
        }
    } else if (operation == "find") {
        if (findNode(bstRoot, value)) {
            response = "Value found in the BST.";
        } else {
            response = "Value not found in the BST.";
        }
    } else {
        response = "Error: Invalid command.";
    }

    send(clientSocket, response.c_str(), response.size(), 0);
}

// Server program entry point
int main() {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error("Failed to create server socket.");
    }

    // Server details
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(1234); // Replace with the desired port number

    // Bind the socket to the specified IP address and port number
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        throw std::runtime_error("Failed to bind server socket to port.");
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0) {
        throw std::runtime_error("Failed to listen on server socket.");
    }

    std::cout << "Server is listening on IP: 127.0.0.1, Port: 1234" << std::endl;

    Node* bstRoot = nullptr;

    // Accept connections and handle client requests
    while (true) {
        // Accept a client connection
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            throw std::runtime_error("Failed to accept client connection.");
        }

        // Get the client's IP and port number
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        getpeername(clientSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);

        // Print client connection details
       // std::cout << "Client connected from IP: " << inet_ntop(clientAddress.sin_addr)
       //           << ", Port: " << ntohs(clientAddress.sin_port) << std::endl;

        // Handle client's commands
        char buffer[1024];
        while (true) {
            // Receive command from the client
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string command(buffer);

                // Handle the client's command
               handleCommand(clientSocket, bstRoot, command);

                // TODO: Implement the logic to send the appropriate response back to the client

                // Exit loop if the command is "exit"
                if (command == "exit") {
                    break;
                }
            } else if (bytesRead == 0) {
                std::cout << "Client closed the connection" << std::endl;
                break;
            } else {
                throw std::runtime_error("Failed to receive data from client");
            }
        }

        // Close the client socket
        close(clientSocket);
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
