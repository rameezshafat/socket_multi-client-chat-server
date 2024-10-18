Chat Group Application in C
Project Overview
This project demonstrates a multi-client chat group application using C with Berkeley Sockets and POSIX threads. It involves a server that can handle multiple clients simultaneously, allowing them to send and receive messages in real-time. This project emphasizes my proficiency in network programming, multi-threading, and low-level system development.

The project is a practical implementation of socket programming concepts, with extensive usage of threading to manage multiple client connections efficiently. It follows the TCP/IP model for reliable data transfer between clients and the server.

Key Features
Multi-client handling: Allows multiple clients to connect to a server and send messages concurrently.
Message broadcasting: All connected clients receive messages sent by others in real-time.
Threading with POSIX: Implemented threads to manage multiple simultaneous connections without blocking the main process.
Socket programming: Utilizes Berkeley sockets for creating, binding, listening, and connecting TCP sockets.
Client identification: Clients are assigned usernames, and messages are broadcast with sender details.
Skills Demonstrated
Network Programming: Deep understanding of how TCP/IP sockets work, client-server architecture, and data transmission.
Concurrency & Threading: Used pthread to create multi-threaded environments, ensuring non-blocking I/O for multiple clients.
System-Level C Programming: Worked with system calls such as socket(), bind(), listen(), and accept() to manage network communication at a low level.
Process Synchronization: Handled multiple clients while maintaining efficient resource utilization.
Error Handling & Debugging: Basic error checks and clean process shutdowns for memory and resource management.
Technologies
Programming Language: C
Libraries: POSIX Threads (pthreads), Berkeley Sockets (socket API)
Build System: CMake (optional), GCC for manual compilation
Platform: Unix/Linux, macOS (can be ported to Windows with adjustments to socket functions)
How It Works
The server listens on a specified port (default: 2000) for incoming client connections.
Each client connects to the server, provides a username, and can start sending messages.
The server uses multi-threading to handle each client's messages concurrently and broadcasts them to other connected clients.
When a client sends a message, it is relayed by the server to all other clients with the sender's username prepended to the message.
The chat continues in real-time until the client disconnects.
Usage Instructions
Prerequisites
Operating System: Linux, macOS, or Windows Subsystem for Linux (WSL)
C Compiler: GCC (or Clang)
CMake (Optional): For streamlined project building
Compilation and Execution
gcc -o socket_server server.c socketutil.c -lpthread
gcc -o socket_client client.c socketutil.c -lpthread
Running the Application
Start the Server: Run the server on a specified port (default: 2000).

./socket_server
Connect Clients: Run the client from a separate terminal or machine.

./socket_client
Chat: Enter your name when prompted and start chatting with other connected clients. Messages from each client will be broadcast to all others.
