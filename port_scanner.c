#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

int scanning_ports(const char *ip, int port){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    target.sin_addr.s_addr = inet_addr(ip);

    if (target.sin_addr.s_addr == INADDR_NONE)
    {
        fprintf(stderr, "Invalid IP address: %s\n", ip);
        close(sock);
        return -1;
    }

    // struct timeval timeout;
    // timeout.tv_sec = 2;
    // timeout.tv_usec = 0;
    
    // setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if(connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0){
        printf("Port %d is open\n", port);
        close(sock);
        return 1;
    }else{
        close(sock);
        return 0;
    } 
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
          fprintf(stderr, "Usage: %s <IP> <start_port> <end_port>\n", argv[0]);
          exit(EXIT_FAILURE);    
    }
    const char *target_ip = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    if (start_port < 1 || end_port > 65535 || start_port > end_port)
    {
        fprintf(stderr, "Invalid port range. Use 1-65535.\n");
        exit(EXIT_FAILURE);
    }
    printf("Scanning %s from port %d to %d...\n", target_ip, start_port, end_port);
    for (int port = start_port; port < end_port; port++)
    {
        printf("\rScanning port %d - ", port);
        fflush(stdout);
        scanning_ports(target_ip, port);
    }
    
    return 0;
}

