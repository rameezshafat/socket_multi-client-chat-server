#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "socketutil.h"

#define MAX_CLIENTS 10

struct AcceptedSocket {
    int acceptedSocketFD;
    struct sockaddr_in address;
    int error;
    bool acceptedSuccessfully;
};

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD);
void receiveAndPrintIncomingData(int *socketFD);
void startAcceptingIncomingConnections(int serverSocketFD);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);
void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD);

// Array to store accepted clients
struct AcceptedSocket acceptedSockets[MAX_CLIENTS];
int acceptedSocketsCount = 0;

// Mutex to protect access to shared data (acceptedSockets)
pthread_mutex_t socketsMutex = PTHREAD_MUTEX_INITIALIZER;

void startAcceptingIncomingConnections(int serverSocketFD) {
    while (true) {
        struct AcceptedSocket *clientSocket = acceptIncomingConnection(serverSocketFD);

        if (clientSocket->acceptedSuccessfully) {
            pthread_mutex_lock(&socketsMutex);
            acceptedSockets[acceptedSocketsCount++] = *clientSocket;
            pthread_mutex_unlock(&socketsMutex);

            receiveAndPrintIncomingDataOnSeparateThread(clientSocket);
        } else {
            fprintf(stderr, "Failed to accept connection, error code: %d\n", clientSocket->error);
            free(clientSocket);
        }
    }
}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket) {
    pthread_t id;
    int *socketFDPtr = malloc(sizeof(int));
    *socketFDPtr = pSocket->acceptedSocketFD;

    if (pthread_create(&id, NULL, (void *(*)(void *)) receiveAndPrintIncomingData, socketFDPtr) != 0) {
        perror("Failed to create thread");
    }
    pthread_detach(id); // Detach the thread to free resources once it's done
}

void receiveAndPrintIncomingData(int *socketFDPtr) {
    int socketFD = *socketFDPtr;
    char buffer[1024];

    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, sizeof(buffer), 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = '\0'; // Null-terminate the received message
            printf("Received: %s\n", buffer);

            sendReceivedMessageToTheOtherClients(buffer, socketFD);
        } else if (amountReceived == 0) {
            printf("Client disconnected.\n");
            break; // Connection closed by client
        } else {
            perror("Failed to receive message");
            break;
        }
    }

    close(socketFD);
    free(socketFDPtr); // Free the memory allocated for the socket FD
}

void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD) {
    pthread_mutex_lock(&socketsMutex);
    for (int i = 0; i < acceptedSocketsCount; i++) {
        if (acceptedSockets[i].acceptedSocketFD != socketFD) {
            ssize_t sent = send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0);
            if (sent == -1) {
                perror("Failed to send message to client");
            }
        }
    }
    pthread_mutex_unlock(&socketsMutex);
}

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD) {
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *) &clientAddress, &clientAddressSize);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->address = clientAddress;
    acceptedSocket->acceptedSocketFD = clientSocketFD;
    acceptedSocket->acceptedSuccessfully = clientSocketFD >= 0;

    if (!acceptedSocket->acceptedSuccessfully) {
        acceptedSocket->error = clientSocketFD;
    }

    return acceptedSocket;
}

int main() {
    int serverSocketFD = createTCPIpv4Socket();
    struct sockaddr_in *serverAddress = createIPv4Address("127.0.0.1", 2000);

    int result = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(*serverAddress));
    if (result == 0) {
        printf("Socket bound successfully\n");
    } else {
        perror("Bind failed");
        return 1;
    }

    int listenResult = listen(serverSocketFD, MAX_CLIENTS);
    if (listenResult == 0) {
        printf("Listening for incoming connections...\n");
    } else {
        perror("Listen failed");
        return 1;
    }

    startAcceptingIncomingConnections(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    close(serverSocketFD);

    return 0;
}
