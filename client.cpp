#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Connect to server
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);  // Use the same port number as the server
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Failed to set server address." << std::endl;
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to connect to the server." << std::endl;
        return 1;
    }

    // Input a number
    int number;
    std::cout << "Enter a number: ";
    std::cin >> number;

    // Send number to server
    if (send(clientSocket, &number, sizeof(number), 0) == -1) {
        std::cerr << "Failed to send number to server." << std::endl;
        return 1;
    }

    // Close socket
    close(clientSocket);

    return 0;
}
