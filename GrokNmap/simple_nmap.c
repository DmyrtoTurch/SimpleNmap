#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

// Function to scan a single port
int scan_port(const char *ip, int port, int timeout_sec) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set timeout for connection
    struct timeval timeout;
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // Set up target address
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &target.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        return -1;
    }

    // Attempt to connect
    int result = connect(sock, (struct sockaddr *)&target, sizeof(target));
    close(sock);

    if (result == 0) {
        return 1; // Port is open
    } else {
        return 0; // Port is closed or filtered
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <IP> <start_port> <end_port>\n", argv[0]);
        fprintf(stderr, "Example: %s 192.168.1.1 20 80\n", argv[0]);
        exit(1);
    }

    const char *ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    if (start_port < 1 || end_port > 65535 || start_port > end_port) {
        fprintf(stderr, "Invalid port range. Ports must be between 1 and 65535.\n");
        exit(1);
    }

    printf("Scanning %s from port %d to %d...\n", ip, start_port, end_port);

    time_t start_time = time(NULL);

    for (int port = start_port; port <= end_port; port++) {
        int result = scan_port(ip, port, 2); // 2-second timeout
        if (result == 1) {
            printf("Port %d: open\n", port);
        } else if (result == -1) {
            fprintf(stderr, "Error scanning port %d\n", port);
        }
    }

    time_t end_time = time(NULL);
    printf("Scan completed in %ld seconds.\n", end_time - start_time);

    return 0;
}