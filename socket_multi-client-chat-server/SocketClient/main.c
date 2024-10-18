#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socketutil.h"

void *listenAndPrint(void *socketFD);

void readConsoleEntriesAndSendToServer(int socketFD);

int main() {

    int socketFD = createTCPIpv4Socket();
    struct sockaddr_in *address = createIPv4Address("127.0.0.1", 2000);

    int result = connect(socketFD, (struct sockaddr *)address, sizeof(*address));

    if (result == 0)
        printf("Connection was successful\n");
    else {
        perror("Connection failed");
        return 1;
    }

    pthread_t listenThread;
    if (pthread_create(&listenThread, NULL, listenAndPrint, &socketFD) != 0) {
        perror("Failed to create thread");
        close(socketFD);
        return 1;
    }

    readConsoleEntriesAndSendToServer(socketFD);

    // Wait for the listener thread to finish
    pthread_join(listenThread, NULL);

    close(socketFD);

    return 0;
}

void readConsoleEntriesAndSendToServer(int socketFD) {
    char *name = NULL;
    size_t nameSize = 0;
    printf("Please enter your name: ");
    ssize_t nameCount = getline(&name, &nameSize, stdin);
    name[nameCount - 1] = 0;  // Remove the newline

    char *line = NULL;
    size_t lineSize = 0;
    printf("Type a message to send (type 'exit' to quit):\n");

    char buffer[1024];

    while (true) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        line[charCount - 1] = 0;  // Remove the newline

        sprintf(buffer, "%s: %s", name, line);

        if (strcmp(line, "exit") == 0)
            break;

        ssize_t amountWasSent = send(socketFD, buffer, strlen(buffer), 0);
        if (amountWasSent == -1) {
            perror("Failed to send message");
            break;
        }
    }

    // Free allocated memory
    free(name);
    free(line);
}

void *listenAndPrint(void *socketFD_ptr) {
    int socketFD = *(int *)socketFD_ptr;
    char buffer[1024];

    while (true) {
        ssize_t amountReceived = recv(socketFD, buffer, 1024, 0);

        if (amountReceived > 0) {
            buffer[amountReceived] = 0;  // Null-terminate the string
            printf("Response: %s\n", buffer);
        } else if (amountReceived == 0) {
            printf("Server closed the connection.\n");
            break;
        } else {
            perror("Failed to receive message");
            break;
        }
    }

    return NULL;
}
