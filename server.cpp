#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Node structure for Binary Search Tree
struct Node {
    int data;
    Node* left;
    Node* right;

    explicit Node(int value) : data(value), left(nullptr), right(nullptr) {}
};

class BSTServer {
public:
    BSTNode* root;

private:

    int acceptClientConnection(int serverSocket) {
        sockaddr_in clientAddress{};
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
        if (clientSocket < 0) {
            throw std::runtime_error("Failed to accept client connection.");
        }

        return clientSocket;
    }

    void handleClientRequests(int clientSocket) {
        char buffer[1024];
        std::string clientMessage;

        while (true) {
            ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                break;
            }

            buffer[bytesRead] = '\0';
            clientMessage += buffer;

            if (clientMessage.find('\n') != std::string::npos) {
                std::string response = processClientCommand(clientMessage);
                send(clientSocket, response.c_str(), response.length(), 0);

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

    bool insertNode(BSTNode*& node, int key) {
        if (node == nullptr) {
            node = new BSTNode(key);
            return true; // Success, key inserted
        }

        if (key < node->key) {
            return insertNode(node->left, key); // Recursively insert in the left subtree
        } else if (key > node->key) {
            return insertNode(node->right, key); // Recursively insert in the right subtree
        } else {
            return false; // Key already exists in the BST
        }
    }

    bool deleteNode(BSTNode*& node, int key) {
        if (node == nullptr) {
            return false; // Key not found
        }

        if (key < node->key) {
            return deleteNode(node->left, key); // Recursively delete in the left subtree
        } else if (key > node->key) {
            return deleteNode(node->right, key); // Recursively delete in the right subtree
        } else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node; // Node has no children
                node = nullptr;
            } else if (node->left == nullptr) {
                BSTNode* temp = node;
                node = node->right; // Node has only right child
                delete temp;
            } else if (node->right == nullptr) {
                BSTNode* temp = node;
                node = node->left; // Node has only left child
                delete temp;
            } else {
                BSTNode* minNode = findMinNode(node->right);
                node->key = minNode->key; // Replace the key with the minimum key in the right subtree
                deleteNode(node->right, minNode->key); // Recursively delete the minimum key from the right subtree
            }

            return true; // Success, key deleted
        }
    }

    bool findNode(BSTNode* node, int key) {
        if (node == nullptr) {
            return false; // Key not found
        }

        if (key < node->key) {
            return findNode(node->left, key); // Recursively search in the left subtree
        } else if (key > node->key) {
            return findNode(node->right, key); // Recursively search in the right subtree
        } else {
            return true; // Key found
        }
    }

    BSTNode* findMinNode(BSTNode* node) {
        while (node->left != nullptr) {
            node = node->left; // Find the minimum key by traversing the left subtree
        }
        return node;
    }
};

int main() {
    BSTServer server;
    server.run(12345); // Start the server

    return 0;
}
