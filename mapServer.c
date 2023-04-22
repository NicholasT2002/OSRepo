#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <time.h> 

#include "common.h"
#include "mapServer.h"

int main(argc, argv)
int argc;
char *argv[];
{
    int sock_fd, new_socket, c;
    char* ip = DEFAULT_IP;
    struct sockaddr_in server, client;

    if (argc > 1)
        ip = argv[1];

    //Create socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Could not create server socket\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(DEFAULT_PORT);

    //Bind socket
    if (bind(sock_fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        perror("Server bind failed\n");

    //Listen
    listen(sock_fd, 3);

    //Accept incoming connections
    c = sizeof(struct sockaddr_in);
    while ((new_socket = accept(sock_fd, (struct sockaddr*)&client, (socklen_t*)&c))) {
        //Do reading/writing here
    }

    exit(0);
}
