// Listening to a given port no 8888 and printing the incoming messages
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>

const int BUFFER_SIZE = 1024;
int NUM_THREADS = 2;

// Structure to store client socket and its details
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    ClientInfo(int socket, struct sockaddr_in& address) : socket(socket), address(address) {}
    ClientInfo() {};
};

// Function to handle a client connection
void *handleClient(void *arg) {
    ClientInfo *clientInfo = static_cast<ClientInfo *>(arg);
    char buffer[BUFFER_SIZE];

    std::cout << "Connected to client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;

    while (true) {
        // Receive data from the client
        ssize_t bytesRead = recv(clientInfo->socket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            // Error or connection closed
            if (bytesRead == 0) {
                std::cout << "Connection closed by client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << std::endl;
            } else {
                perror("Recv error");
            }
            break;
        } else {
            // Print the received message
            buffer[bytesRead] = '\0';
            std::cout << "Received message from client, IP: " << inet_ntoa(clientInfo->address.sin_addr) << ", Port: " << ntohs(clientInfo->address.sin_port) << ": " << buffer << std::endl;
        }
    }

    // Close the client socket
    close(clientInfo->socket);
    delete clientInfo;
    pthread_exit(NULL);
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // Create server socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);  // Port number
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind server socket to the specified address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {  // Maximum 5 pending connections
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    std::cout << "Trader is listening on port 8888..." << std::endl;

    std::vector<pthread_t> clientThreads;

    for(int i = 0; i < NUM_THREADS; i++) {
        // Accept incoming connections
        int clientSocket;
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen)) == -1) {
            perror("Accept error");
            continue;  // Continue listening for other connections
        }

        // Create a thread to handle this client
        ClientInfo *clientInfo = new ClientInfo(clientSocket, clientAddr);
        pthread_t clientThread;
        if (pthread_create(&clientThread, NULL, handleClient, clientInfo) != 0) {
            perror("Thread creation error");
            delete clientInfo;
            continue;  // Continue listening for other connections
        }

        // Store the thread ID for later joining
        clientThreads.push_back(clientThread);
    }

    // Join all client threads (clean up)
    for (auto &thread : clientThreads) {
        pthread_join(thread, NULL);
    }

    // Close the server socket (never reached in this example)
    close(serverSocket);

    return 0;
}
