#include <stdio.h>      // Standard input/output functions (for printf, perror)
#include <stdlib.h>     // Standard library functions (for exit)
#include <string.h>     // String manipulation functions (for strlen)
#include <unistd.h>     // POSIX operating system API (for close, read, write)
#include <arpa/inet.h>  // Functions for manipulating IP addresses (for inet_ntoa, htons)
#include <sys/socket.h> // Core socket functions (for socket, bind, listen, accept)

#define PORT 8585       // The port number the server will listen on
#define BUFFER_SIZE 1024 // Size of the buffer for receiving data
#define BACKLOG 5       // Maximum number of pending connections queue

int main() {
    int server_sock_fd, client_sock_fd; // Socket file descriptors for server and client
    struct sockaddr_in server_addr, client_addr; // Structures to hold socket addresses
    socklen_t client_addr_len; // Length of the client address structure
    char buffer[BUFFER_SIZE];  // Buffer to store received and sent data
    int bytes_read;            // Number of bytes read from the client

    // 1. Create a socket
    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams (TCP)
    // 0: Protocol (IP)
    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        perror("Error: Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully (File Descriptor: %d).\n", server_sock_fd);

    // Optional: Allow reuse of address and port immediately after closing.
    // This helps prevent "Address already in use" errors if you restart the server quickly.
    int optval = 1;
    if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("Warning: setsockopt SO_REUSEADDR failed");
    }

    // 2. Prepare the sockaddr_in structure (server address)
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT); // Port number, converted to network byte order
    // INADDR_ANY: Bind to all available network interfaces on the host
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind the socket to the specified IP address and port
    if (bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: Could not bind socket");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d successfully.\n", PORT);

    // 4. Listen for incoming connections
    // BACKLOG: Maximum number of connection requests that can be queued
    if (listen(server_sock_fd, BACKLOG) == -1) {
        perror("Error: Could not listen on socket");
        close(server_sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // 5. Accept incoming connections in a loop
    while (1) {
        client_addr_len = sizeof(client_addr);
        // accept(): Blocks until a client connects. Returns a new socket file descriptor for the client connection.
        client_sock_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock_fd == -1) {
            perror("Error: Could not accept client connection");
            // Don't exit here, continue to listen for other clients
            continue;
        }

        // Print client information
        printf("Accepted connection from %s:%d (Client FD: %d)\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_sock_fd);

        // 6. Read data from the client
        // Clear the buffer before reading
        memset(buffer, 0, BUFFER_SIZE);
        bytes_read = read(client_sock_fd, buffer, BUFFER_SIZE - 1); // -1 to leave space for null terminator
        if (bytes_read == -1) {
            perror("Error: Could not read from client");
        } else if (bytes_read == 0) {
            printf("Client disconnected.\n");
        } else {
            // Null-terminate the received data to treat it as a string
            buffer[bytes_read] = '\0';
            printf("Received from client: %s\n", buffer);

            // 7. Echo the data back to the client
            if (write(client_sock_fd, buffer, bytes_read) == -1) { // Write original bytes_read, not strlen(buffer)
                perror("Error: Could not write to client");
            } else {
                printf("Echoed back to client: %s\n", buffer);
            }
        }

        // 8. Close the client connection
        close(client_sock_fd);
        printf("Client connection closed (Client FD: %d).\n", client_sock_fd);
    }

    // This part is technically unreachable in the infinite loop, but good practice for completeness
    close(server_sock_fd);
    return 0;
}
