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
