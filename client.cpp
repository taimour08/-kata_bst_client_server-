#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main() {
    BSTClient client;
    client.run("127.0.0.1", 12345); // Connect to the server at IP address "127.0.0.1" and port 12345

    return 0;
}
}
